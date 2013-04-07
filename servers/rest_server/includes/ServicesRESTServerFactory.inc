<?php

/**
 * Factory class to build RESTServer object.
 *
 * If you want to change it
 */

class ServicesRESTServerFactory {
  public static function getRESTServer() {
    $content_type_negotiator = self::getContentTypeNegotiator();
    return new RESTServer($content_type_negotiator);
  }

  protected static function getContentTypeNegotiator() {
    return new ServicesContentTypeNegotiator();
  }
}