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
    return new RESTServer($context, $content_type_negotiator);
  }

  protected static function getContentTypeNegotiator() {
    return new ServicesContentTypeNegotiator();
  }

  protected static function getContext($endpoint_path) {
    $context = new ServicesContext($endpoint_path);
    $context->buildFromGlobals();
    return $context;
  }
}