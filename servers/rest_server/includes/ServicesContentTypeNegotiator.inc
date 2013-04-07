<?php

interface ServicesContentTypeNegotiatorInterface {
  public function getResponseFormatFromURL(&$canonical_path);
  public function getResponseFormatContentTypeNegotiations(&$mime_type, $canonical_path, $controller, $formats);
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
  public function getResponseFormatFromURL(&$canonical_path) {
    $matches = array();
    $response_format = '';
    if (preg_match('/^(.+)\.([^\.^\/]+)$/', $canonical_path, $matches)) {
      $canonical_path = $matches[1];
      $response_format = $matches[2];
    }

    return $response_format;
  }

  /**
   * Determine response format and mime type using headers to negotiate content types.
   */
  public function getResponseFormatContentTypeNegotiations(&$mime_type, $canonical_path, $controller, $formats) {
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
    if (isset($_SERVER['HTTP_ACCEPT'])) {
      $mime = $this->mimeParse();
      $mime_type = $mime->best_match($mime_candidates, $_SERVER['HTTP_ACCEPT']);
      $response_format = $mime_map[$mime_type];
    }

    return $response_format;
  }

  protected function supportedControllerModel($controller, $format) {
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
  protected function mimeParse() {
    static $mimeparse;
    if (!$mimeparse) {
      module_load_include('php', 'rest_server', 'lib/mimeparse');
      $mimeparse = new Mimeparse();
    }
    return $mimeparse;
  }
}