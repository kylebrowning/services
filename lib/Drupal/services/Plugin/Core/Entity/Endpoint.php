<?php

/**
 * @file
 * Contains \Drupal\services\Plugin\Core\Entity\Endpoint.
 */

namespace Drupal\services\Plugin\Core\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Entity\Annotation\EntityType;
use Drupal\Core\Annotation\Translation;

/**
 * Defines an services endpoint configuration entity.
 *
 * @EntityType(
 *   id = "endpoint",
 *   label = @Translation("Endpoint"),
 *   module = "services",
 *   controllers = {
 *     "storage" = "Drupal\services\EndpointStorageController",
 *     "list" = "Drupal\views_ui\ViewListController",
 *     "form" = {
 *       "edit" = "Drupal\views_ui\ViewEditFormController",
 *       "add" = "Drupal\views_ui\ViewAddFormController",
 *       "preview" = "Drupal\views_ui\ViewPreviewFormController",
 *       "clone" = "Drupal\views_ui\ViewCloneFormController"
 *     }
 *   },
 *   config_prefix = "services.endpoint",
 *   fieldable = FALSE,
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid",
 *     "status" = "status"
 *   }
 * )
 */
class Endpoint extends ConfigEntityBase {

  /**
   * Label.
   *
   * @var string
   */
  public $label;

  /**
   * Path of the endpoint.
   *
   * @var string
   */
  public $path;

  /**
   * Authentication settings.
   *
   * @var array
   */
  public $authentication;

  /**
   * Server settings like formatters, parsers enabled.
   *
   * @var array
   */
  public $settings;

  /**
   * Whether debug mode enabled.
   *
   * @var boolean
   */
  public $debug;
}
