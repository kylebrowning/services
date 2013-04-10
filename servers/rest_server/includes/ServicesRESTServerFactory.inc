<?php

/**
 * Factory class to build RESTServer object.
 *
 * If you want to change it
 */

class ServicesRESTServerFactory {
  protected $data = array();

  /**
   * We need data property to pass additional arguments to methods.
   */
  public function __construct($data = array()) {
    $this->data = $data;
  }

  public function getRESTServer($endpoint_path) {
    $content_type_negotiator = self::getContentTypeNegotiator();
    $context = $this->getContext($endpoint_path);
    $resources = $this->getResources();
    $endpoint = $this->getEndpoint();
    $parsers = $this->getParsers();
    $formatters = $this->getFormatters();
    return new RESTServer($context, $content_type_negotiator, $resources, $endpoint, $parsers, $formatters);
  }

  protected function getContentTypeNegotiator() {
    return new ServicesContentTypeNegotiator();
  }

  protected function getContext($endpoint_path) {
    $context = new ServicesContext($endpoint_path);
    $context->buildFromGlobals();
    return $context;
  }

  protected function getResources() {
    $endpoint_name = services_get_server_info('endpoint', '');
    $endpoint = services_endpoint_load($endpoint_name);
    $resources = services_get_resources($endpoint->name);
    module_load_include('inc', 'services', 'includes/services.resource_build');
    _services_apply_endpoint($resources, $endpoint, TRUE);

    return $resources;
  }

  protected function getEndpoint() {
    $endpoint_name = services_get_server_info('endpoint', '');
    return services_endpoint_load($endpoint_name);
  }

  protected function getEndpointSettings() {
    static $settings;

    if (empty($settings)) {
      $endpoint = $this->getEndpoint();

      // Get the server settings from the endpoint.
      $settings = !empty($endpoint->server_settings) ? $endpoint->server_settings : array();
      // Normalize the settings so that we get the expected structure
      // and sensible defaults.
      $settings = rest_server_setup_settings($settings);
    }

    return $settings;
  }

  protected function getParsers() {
    $settings = $this->getEndpointSettings();

    $parsers = rest_server_request_parsers();
    // Remove parsers that have been disabled for this endpoint.
    foreach (array_keys($parsers) as $key) {
      if (!$settings['parsers'][$key]) {
        unset($parsers[$key]);
      }
    }

    return $parsers;
  }

  protected function getFormatters() {
    $settings = $this->getEndpointSettings();

    $formatters = rest_server_response_formatters();
    // Remove formatters that have been disabled for this endpoint.
    foreach (array_keys($formatters) as $key) {
      if (!$settings['formatters'][$key]) {
        unset($formatters[$key]);
      }
    }

    return $formatters;
  }
}