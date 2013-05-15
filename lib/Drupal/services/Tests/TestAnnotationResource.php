<?php

namespace Drupal\services\Tests;

use Drupal\services\Plugin\AnnotationResourceBase;
use Drupal\services\Annotation\ResourceMethod;

class TestAnnotationResource extends AnnotationResourceBase {

  /**
   * Custom call to check annotation reader.
   *
   * @ResourceMethod(
   *   httpMethod = "POST",
   *   uri = "customCall"
   * )
   */
  public function customCall($arg1, $arg2) {

  }

  /**
   * Function without annotation.
   */
  public function noAnnotationFunction() {

  }
}