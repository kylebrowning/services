<?php
/**
 * @file
 * Provides Drupal\services\Annotation\ServiceArgument.
 */

namespace Drupal\services;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a ServiceArgument annotation object.
 *
 * Service arguments handle all metadata used to descrive request arguments.
 *
 * @Annotation
 */

class ServiceArgument extends Plugin {

  /**
   * The argument ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the service argument.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $title;

  /**
   * The service argument is required or not.
   *
   * @var boolean
   */
  public $required;

  /**
   * The message if the argument fails.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $error_message;

}
