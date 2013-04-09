<?php

/**
 * Factory class to build RESTServer object.
 *
 * If you want to change it
 */

class ServicesRESTServerFactory {
  public static function getRESTServer($endpoint_path) {
    $content_type_negotiator = self::getContentTypeNegotiator();
    $context = self::getContext($endpoint_path);
    $resources = self::getResources();
    return new RESTServer($context, $content_type_negotiator, $resources);
  }

  protected static function getContentTypeNegotiator() {
    return new ServicesContentTypeNegotiator();
  }

  protected static function getContext($endpoint_path) {
    $context = new ServicesContext($endpoint_path);
    $context->buildFromGlobals();
    return $context;
  }

  protected static function getResources() {
    $endpoint_name = services_get_server_info('endpoint', '');
    $endpoint = services_endpoint_load($endpoint_name);
    $resources = services_get_resources($endpoint->name);
    module_load_include('inc', 'services', 'includes/services.resource_build');
    _services_apply_endpoint($resources, $endpoint, TRUE);

    return $resources;
  }
}