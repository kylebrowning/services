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
   * The human-readable name of the service category.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $category;

  /**
   * The appended path from the endpoint.
   *
   * @var string
   */
  public $path;


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

  /**
   * The service definition contexts.
   *
   * @var \Drupal\Core\Annotation\ContextDefinition[]
   */
  public $contexts;

}
