<?php
/**
 * @file
 * Provides Drupal\services\Annotation\ServiceResponse.
 */

namespace Drupal\services\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a ServiceResponse annotation object.
 *
 * ServiceResponse handles all metadata used to describe ServiceResponse plugins.
 *
 * @Annotation
 */

class ServiceResponse extends Plugin {

  /**
   * The argument ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the service response.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $label;

}
