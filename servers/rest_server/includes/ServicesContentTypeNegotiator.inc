<?php

interface ServicesContentTypeNegotiatorInterface {
  public function getResponseFormatFromURL($canonical_path);
  public function getResponseFormatContentTypeNegotiations(&$mime_type, $canonical_path, $controller, $formats, $context);
}

/**
 * Class used to do Content Type negotiation.
 */

class ServicesContentTypeNegotiator implements ServicesContentTypeNegotiatorInterface {

  /**
   * Retrieve formatter from URL. If format is in the path, we remove it from $canonical_path.
   *
   * For example <endpoint>/<path>.<format>
   *
   * @param $canonical_path
   *
   * @return string
   */
  public function getResponseFormatFromURL($canonical_path) {
    $matches = $this->getCanonicalPathMatches($canonical_path);
    return $matches[2];
  }

  public function getParsedCanonicalPath($canonical_path) {
    $matches = $this->getCanonicalPathMatches($canonical_path);
    return $matches[1];
  }

  public function getCanonicalPathMatches($canonical_path) {
    $matches = array();
    if (preg_match('/^(.+)\.([^\.^\/]+)$/', $canonical_path, $matches)) {
      return $matches;
    }
    return array('', $canonical_path, '');
  }

  /**
   * Determine response format and mime type using headers to negotiate content types.
   */
  public function getResponseFormatContentTypeNegotiations(&$mime_type, $canonical_path, $controller, $formats, $context) {
    drupal_add_http_header('Vary', 'Accept');

    // Negotiate response format based on accept-headers if we
    // don't have a response format.
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
    $response_format = 'json';
    $http_accept = $context->getServerVariable('HTTP_ACCEPT');
    if (!empty($http_accept)) {
      $mime = $this->mimeParse();
      $mime_type = $mime->best_match($mime_candidates, $http_accept);
      $response_format = $mime_map[$mime_type];
    }

    return $response_format;
  }

  public function supportedControllerModel($controller, $format) {
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

  /**
   * Create a instance of the Mimeparse utility class.
   *
   * @return Mimeparse
   */
  public function mimeParse() {
    static $mimeparse;
    if (!$mimeparse) {
      module_load_include('php', 'rest_server', 'lib/mimeparse');
      $mimeparse = new Mimeparse();
    }
    return $mimeparse;
  }
}