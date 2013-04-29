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
 *     "list" = "Drupal\services\EndpointListController",
 *     "form" = {
 *       "edit" = "Drupal\views_ui\ViewEditFormController",
 *       "add" = "Drupal\services\EndpointAddFormController",
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
   * ID.
   *
   * @var string
   */
  public $id;

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

  /**
   * Overrides Drupal\Core\Entity\EntityInterface::uri().
   */
  public function uri() {
    return array(
      'path' => 'admin/structure/services/endpoint/' . $this->id(),
      'options' => array(
        'entity_type' => $this->entityType,
        'entity' => $this,
      ),
    );
  }

  /**
   * Overrides \Drupal\Core\Config\Entity\ConfigEntityBase::getExportProperties();
   */
  public function getExportProperties() {
    $names = array(
      'label',
      'path',
      'authentication',
      'settings',
      'debug',
      'label',
      'id',
      'uuid',
    );
    $properties = array();
    foreach ($names as $name) {
      $properties[$name] = $this->get($name);
    }
    return $properties;
  }

}
