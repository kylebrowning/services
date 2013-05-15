<?php

/**
 * @file Annotation description for ResourceMethod.
 */

namespace Drupal\services\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines an ResourceMethod annotation object.
 *
 * @Annotation
 */
class ResourceMethod extends Plugin {

  /**
   * HTTP method name.
   *
   * @var string
   */
  public $httpMethod = 'GET';
}