<?php

/**
 * @file
 * Class for handling REST calls.
 */

class RESTServer {
  private $endpoint;

  /**
   * Handles the call to the REST server
   *
   * @param string $canonical_path
   * @param string $endpoint_path
   * @return void
   */
  public function handle($canonical_path, $endpoint_path) {
    $this->endpoint_path = $endpoint_path;
    services_set_server_info('resource_uri_formatter', array(&$this, 'uri_formatter'));

    // Determine the request method
    $method = $_SERVER['REQUEST_METHOD'];
    if ($method=='POST' && isset($_SERVER['HTTP_X_HTTP_METHOD_OVERRIDE'])) {
      $method = $_SERVER['HTTP_X_HTTP_METHOD_OVERRIDE'];
    }
    if ($method=='POST' && (isset($_GET['_method']) && $_GET['_method'])) {
      $method = $_GET['_method'];
    }
    $path = explode('/', $canonical_path);
    $leaf = array_pop($path);
    $resource_name = array_shift($path);
    // Extract response format info from the path
    $matches=array();
    if ($leaf && preg_match('/^(.+)\.([^\.]+)$/', $leaf, $matches)) {
      $leaf = $matches[1];
      $response_format = $matches[2];
    }
    // Response will vary with accept headers
    // if no format was supplied as path suffix
    if (empty($response_format)) {
      drupal_set_header('Vary: Accept');
    }

    // Return the leaf to the path array if it's not the resource name
    if ($leaf) {
      if (!$resource_name) {
        $resource_name = $leaf;
      }
      else {
        array_push($path, $leaf);
      }
    }

    $endpoint = services_get_server_info('endpoint', '');
    $resources = services_get_resources($endpoint);
    $controller = FALSE;
    if (!empty($resource_name) && isset($resources[$resource_name])) {
      $resource = $resources[$resource_name];

      // Get the operation and fill with default values
      $controller = $this->resolveController($resource, $method, $path);
    }
    else {
      throw new ServicesException(t('Could not find resource @name', array(
          '@name' => $resource_name,
        )), 404);
    }

    if (!$controller) {
      throw new ServicesException('Could not find the controller', 404);
    }

    // Parse the request data
    $data = array();
    $arguments = $this->getControllerArguments($controller, $path, $method, $data);

    // Any authentication needed for REST Server must be set in the cookies
    $auth_arguments = $_COOKIE;

    $formats = $this->responseFormatters();

    // Negotiate response format based on accept-headers if we
    // don't have a response format
    if (empty($response_format)) {
      module_load_include('php', 'rest_server', 'lib/mimeparse');

      $mime_candidates = array();
      $mime_map = array();
      // Add all formatters that accepts raw data, or supports the format model
      foreach ($formats as $format => $formatter) {
        if (!isset($formatter['model']) || $this->supportedControllerModel($controller, $formatter)) {
          foreach ($formatter['mime types'] as $m) {
            $mime_candidates[] = $m;
            $mime_map[$m] = $format;
          }
        }
      }

      // Get the best matching format, default to json
      if (isset($_SERVER['HTTP_ACCEPT'])) {
        $mime = new Mimeparse();
        $mime_type = $mime->best_match($mime_candidates, $_SERVER['HTTP_ACCEPT']);
      }

      if (isset($mime_type) && $mime_type) {
        $response_format = $mime_map[$mime_type];
      }
      else {
        $response_format = 'json';
      }
    }

    // Check if we support the response format and determine the mime type
    if (empty($mime_type) && !empty($response_format) && isset($formats[$response_format])) {
      $formatter = $formats[$response_format];
      if (!isset($formatter['model']) || $this->supportedControllerModel($controller, $formatter)) {
        $mime_type = $formatter['mime types'][0];
      }
    }

    if (empty($response_format) || empty($mime_type)) {
      throw new ServicesException('Unknown or unsupported response format', 406);
    }

    // Give the model (if any) a opportunity to alter the arguments.
    // This might be needed for the model to ensure that all the required
    // information is requested.
    if (isset($formatter['model'])) {
      $cm = &$controller['models'][$formatter['model']];
      if (!isset($cm['arguments'])) {
        $cm['arguments'] = array();
      }

      // Check if any of the model arguments have been overridden
      if (isset($cm['allow_overrides'])) {
        foreach ($cm['allow_overrides'] as $arg) {
          if (isset($_GET[$formatter['model'] . ':' . $arg])) {
            $cm['arguments'][$arg] = $_GET[$formatter['model'] . ':' . $arg];
          }
        }
      }

      if (isset($cm['class']) && class_exists($cm['class'])) {
        if (method_exists($cm['class'], 'alterArguments')) {
          call_user_func_array($cm['class'] . '::alterArguments', array(&$arguments, $cm['arguments']));
        }
      }
    }

    try {
      $result = services_controller_execute($controller, $arguments);
    } catch (ServicesException $e) {
      $errors = $this->handleException($e);
      drupal_alter('rest_server_exectue_errors', $errors, $controller, $arguments);
      $result = $errors;
    }
    $formatter = $formats[$response_format];

    // Set the content type and render output
    drupal_set_header('Content-type: ' . $mime_type);
    return $this->renderFormatterView($controller, $formatter, $result);
  }

  /**
   * Formats a resource uri
   *
   * @param array $path
   *  An array of strings containing the component parts of the path to the resource.
   * @return string
   *  Returns the formatted resource uri
   */
  public function uri_formatter($path) {
    return url($this->endpoint_path . '/' . join($path, '/'), array(
      'absolute' => TRUE,
    ));
  }

  /**
   * Parses controller arguments from request
   *
   * @param array $controller
   *  The controller definition
   * @param string $path
   * @param string $method
   *  The method used to make the request
   * @param array $sources
   *  An array of the sources used for getting the arguments for the call
   * @return void
   */
  private function getControllerArguments($controller, $path, $method, &$sources=array()) {
    // Get argument sources
    $parameters = $_GET;
    unset($parameters['_method']);
    $data = $this->parseRequest($method, $controller);

    $defaults = array(
      'path' => $path,
      'param' => $parameters,
      'data' => $data,
    );
    foreach ($defaults as $key => $data) {
      if (!isset($sources[$key])) {
        $sources[$key] = $data;
      }
    }
    // Map source data to arguments.
    $arguments = array();
    if (isset($controller['args'])) {
      foreach ($controller['args'] as $i => $info) {
        // Fill in argument from source
        if (isset($info['source'])) {
          if (is_array($info['source'])) {
            list($source) = array_keys($info['source']);
            $key = $info['source'][$source];
            if (isset($sources[$source][$key])) {
              $arguments[$i] = $sources[$source][$key];
            }
          }
          else {
              if (isset($sources[$info['source']][$info['name']])) {
                $arguments[$i] = $sources[$info['source']][$info['name']];
              }
          }
          // Convert to array if argument expected to be array.
          if ($info['type'] == 'array' && isset($arguments[$i])) {
            $arguments[$i] = (array)$arguments[$i];
          }
        }

        // When argument isn't set, insert default value if provided or
        // throw a exception if the argument isn't optional.
        if (!isset($arguments[$i])) {
          if (isset($info['default value'])) {
            $arguments[$i] = $info['default value'];
          }
          elseif (!isset($info['optional']) || !$info['optional']) {
            return services_error(t('Missing required argument !arg', array(
              '!arg' => $info['name'],
            )), 406);
          }
          else {
              $arguments[$i] = NULL;
          }
        }
      }
    }
    return $arguments;
  }

  private function parseRequest($method, $controller) {
    if (($method == 'POST' || $method == 'PUT') &&  isset($_SERVER['CONTENT_TYPE'])) {
      $type = self::parseContentHeader($_SERVER['CONTENT_TYPE']);
    }
    switch ($method) {
      case 'POST':
        if (isset($type['value']) && $type['value'] == 'application/x-www-form-urlencoded') {
          return $_POST;
        }
      case 'PUT':
        // Get the mime type for the request, default to form-urlencoded
        if (isset($type['value'])) {
          $mime = $type['value'];
        }
        else {
          $mime = 'application/x-www-form-urlencoded';
        }

        // Get the parser for the mime type
        $parser = $this->requestParsers($mime, $controller);
        if (!$parser) {
          throw new ServicesException(t('Unsupported request content type !mime', array(
              '!mime' => $mime,
            )), 406);
        }

        // Read the raw input stream
        if (module_exists('inputstream')) {
          $handle = fopen('drupal://input', 'r');
        }
        else {
          $handle = fopen('php://input', 'r');
        }
        if ($handle) {
          $data = call_user_func($parser, $handle);
          fclose($handle);
        }
        return $data;

      default:
        return array();
    }
  }

  public static function parseContentHeader($value) {
    $ret_val = array();
    $value_pattern = '/^([^;]+)(;\s*(.+)\s*)?$/';
    $param_pattern = '/([a-z]+)=(([^\"][^;]+)|(\"(\\\"|[^"])+\"))/';
    $vm=array();

    if (preg_match($value_pattern, $value, $vm)) {
      $ret_val['value'] = $vm[1];
      if (count($vm)>2) {
        $pm = array();
        if (preg_match_all($param_pattern, $vm[3], $pm)) {
          $pcount = count($pm[0]);
          for ($i=0; $i<$pcount; $i++) {
            $value = $pm[2][$i];
            if (drupal_substr($value, 0, 1) == '"') {
              $value = stripcslashes(drupal_substr($value, 1, mb_strlen($value)-2));
            }
            $ret_val['param'][$pm[1][$i]] = $value;
          }
        }
      }
    }

    return $ret_val;
  }

  public static function contentFromStream($handle) {
    $content = '';
    while (!feof($handle)) {
      $content .= fread($handle, 8192);
    }
    return $content;
  }

  public static function fileRecieve($handle, $validators=array()) {
    $validators['file_validate_name_length'] = array();

    $type = RESTServer::parseContentHeader($_SERVER['CONTENT_TYPE']);
    $disposition = RESTServer::parseContentHeader($_SERVER['HTTP_CONTENT_DISPOSITION']);

    $filename = file_munge_filename(trim(basename(($disposition['param']['filename']))));

    // Rename potentially executable files, to help prevent exploits.
    if (preg_match('/\.(php|pl|py|cgi|asp|js)$/i', $filename) && (drupal_substr($filename, -4) != '.txt')) {
      $type['value'] = 'text/plain';
      $filename .= '.txt';
    }

    $filepath = file_destination(file_create_path(file_directory_temp() . '/' . $filename), FILE_EXISTS_RENAME);
    $file = (object)array(
      'uid' => 0,
      'filename' => $filename,
      'filepath' => $filepath,
      'filemime' => $type['value'],
      'status' => FILE_STATUS_TEMPORARY,
      'timestamp' => time(),
    );
    RESTServer::streamToFile($handle, $filepath);
    $file->filesize = filesize($filepath);

    // Call the validation functions.
    $errors = array();
    foreach ($validators as $function => $args) {
      array_unshift($args, $file);
      $errors = array_merge($errors, call_user_func_array($function, $args));
    }
    if (!empty($errors)) {
      throw new ServicesException(t('Errors while validating the file'), 406, $errors);
    }

    drupal_write_record('files', $file);

    return $file;
  }

  public static function streamToFile($source, $file) {
    $fp = fopen($file, 'w');
    if ($fp) {
      self::streamCopy($source, $fp);
      fclose($fp);
      return TRUE;
    }
    return FALSE;
  }

  public static function streamCopy($source, $destination) {
    while (!feof($source)) {
      $content = fread($source, 8192);
      fwrite($destination, $content);
    }
  }

  private function renderFormatterView($controller, $formatter, $result) {
    // Wrap the results in a model class if required by the formatter
    if (isset($formatter['model'])) {
      $cm = $controller['models'][$formatter['model']];
      $model_arguments = isset($cm['arguments'])?$cm['arguments']:array();

      $model_class = new ReflectionClass($cm['class']);
      $result = $model_class->newInstanceArgs(array($result, $model_arguments));
    }

    $view_class = new ReflectionClass($formatter['view']);
    $view_arguments = isset($formatter['view arguments'])?$formatter['view arguments']:array();
    $view = $view_class->newInstanceArgs(array($result, $view_arguments));
    return $view->render();
  }

  private function requestParsers($mime=NULL, $controller=NULL) {
    static $parsers;

    if ($mime && $controller && !empty($controller['rest request parsers'])) {
      $parser = $this->matchParser($mime, $controller['rest request parsers']);
      if ($parser) {
        return $parser;
      }
    }

    if (!$parsers) {
      $parsers = array(
        'application/x-www-form-urlencoded' => 'RESTServer::parseURLEncoded',
        'application/x-yaml' => 'RESTServer::parseYAML',
        'application/json' => 'RESTServer::parseJSON',
        'application/vnd.php.serialized' => 'RESTServer::parsePHP',
      );
      drupal_alter('rest_server_request_parsers', $parsers);
    }

    if ($mime) {
      return $this->matchParser($mime, $parsers);
    }
    return $parsers;
  }

  private function mimeParse() {
    static $mimeparse;
    if (!$mimeparse) {
      module_load_include('php', 'rest_server', 'lib/mimeparse');
      $mimeparse = new Mimeparse();
    }
    return $mimeparse;
  }

  private function matchParser($mime, $parsers) {
    $mimeparse = $this->mimeParse();
    $mime_type = $mimeparse->best_match(array_keys($parsers), $mime);
    if ($mime_type) {
      return $parsers[$mime_type];
    }
  }

  public static function parseURLEncoded($handle) {
    parse_str(self::contentFromStream($handle), $data);
    return $data;
  }

  public static function parsePHP($handle) {
    return unserialize(self::contentFromStream($handle));
  }

  public static function parseJSON($handle) {
    return json_decode(self::contentFromStream($handle), TRUE);
  }

  public static function parseYAML($handle) {
    module_load_include('php', 'rest_server', 'lib/spyc');
    return Spyc::YAMLLoad(self::contentFromStream($handle));
  }

  private function responseFormatters($format=NULL) {
    static $formatters;

    if (!$formatters) {
      $formatters = array(
        'xml' => array(
          'mime types' => array('application/xml', 'text/xml'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'xml'),
        ),
        'json' => array(
          'mime types' => array('application/json'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'json'),
        ),
        'jsonp' => array(
          'mime types' => array('text/javascript', 'application/javascript'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'jsonp'),
        ),
        'php' => array(
          'mime types' => array('application/vnd.php.serialized'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'php'),
        ),
        'yaml' => array(
          'mime types' => array('text/plain', 'application/x-yaml', 'text/yaml'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'yaml'),
        ),
        'bencode' => array(
          'mime types' => array('application/x-bencode'),
          'view' => 'RESTServerViewBuiltIn',
          'view arguments' => array('format' => 'bencode'),
        ),
        'rss' => array(
          'model' => 'ResourceFeedModel',
          'mime types' => array('text/xml'),
          'view' => 'RssFormatView',
        ),
      );
      drupal_alter('rest_server_response_formatters', $formatters);
    }

    if ($format) {
      return isset($formatters[$format]) ? $formatters[$format] : FALSE;
    }
    return $formatters;
  }

  private function loadInclude($file) {
    module_load_include($file['file'], $file['module'], isset($file['name'])?$file['name']:NULL);
  }

  private function supportedControllerModel($controller, $format) {
    if (
      // The format uses models
      isset($format['model']) &&
      // The controller provides models
      isset($controller['models']) &&
      // The controller supports the model required by the format
      isset($controller['models'][$format['model']])) {
        return $controller['models'][$format['model']];
    }
  }

  function handleException($e){
    $code = $e->getCode();
    switch ($code) {
      case 401:
        drupal_set_header('HTTP/1.0 401 Unauthorized: ' . $e->getMessage());
      break;
      case 404:
        drupal_set_header('HTTP/1.0 404 Not found: ' . $e->getMessage());
      break;
      case 406:
        drupal_set_header('HTTP/1.0 406 Not Acceptable: ' . $e->getMessage());
      break;
      default:
        if ($code >= 400 && $code < 600) {
          drupal_set_header('HTTP/1.0 ' . $code . ' ' . $e->getMessage());
        }
        else {
          drupal_set_header('HTTP/1.0 500 An error occurred: (' . $code . ') ' . $e->getMessage());
        }
      break;
    }
    if(method_exists($e,'getData')){
      return $e->getData();
    }
  }
  private function resolveController($resource, $method, $path) {
    $pc = count($path);
    // Use the index handler for all empty path request, except on POST
    if (!$pc && $method!='POST') {
      return isset($resource['index']) ? $resource['index'] : NULL;
    }
    // Detect the standard crud operations
    elseif (
        ($pc==1 && ($method=='GET' || $method=='PUT' || $method=='DELETE')) ||
        ($pc==0 && $method='POST')) {
      $action_mapping = array(
        'GET' => 'retrieve',
        'POST' => 'create',
        'PUT' => 'update',
        'DELETE' => 'delete',
      );
      if (isset($resource[$action_mapping[$method]])) {
        $controller = $resource[$action_mapping[$method]];
        if (isset($resource['file'])) {
          $controller['file'] = $resource['file'];
        }
        return $controller;
      }
    }
    // Detect relationship requests
    elseif ($pc >= 2 && $method == 'GET') {
      if (isset($resource['relationships']) && $resource['relationships'][$path[1]]) {
        $relationship = $resource['relationships'][$path[1]];
        return $relationship;
      }
    }
    // Detect action requests
    elseif ($pc == 1 && $method == 'POST') {
      return $resource['actions'][$path[0]];
    }
    // Detect action requests targeted at specific resources
    elseif ($pc >= 2 && $method == 'POST') {
      $action = $resource['targeted actions'][$path[1]];
      return $action;
    }
  }
}