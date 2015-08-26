<?php

/**
 * @file
 * Contains Drupal\services\Annotation\ServiceDefinition.
 */

namespace Drupal\services\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a service definition annotation object.
 *
 * @Annotation
 */
class ServiceDefinition extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the service definition.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $title;

  /**
   * The description shown to users.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $description;


  /**
   * The service definition supports translations.
   *
   * @var boolean
   */
  public $translatable;

}
