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
    return new RESTServer($context, $content_type_negotiator, $resources);
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
}