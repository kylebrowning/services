<?php

/**
 * @file
 * Contains Drupal\services\Entity\ServiceAPI.
 */

namespace Drupal\services\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\services\ServiceAPIInterface;

/**
 * Defines the Service api entity.
 *
 * @ConfigEntityType(
 *   id = "service_api",
 *   label = @Translation("Service api"),
 *   handlers = {
 *     "list_builder" = "Drupal\services\Controller\ServiceAPIListBuilder",
 *     "form" = {
 *       "add" = "Drupal\services\Form\ServiceAPIForm",
 *       "edit" = "Drupal\services\Form\ServiceAPIForm",
 *       "delete" = "Drupal\services\Form\ServiceAPIDeleteForm"
 *     }
 *   },
 *   config_prefix = "service_api",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/service_api/{service_api}",
 *     "edit-form" = "/admin/structure/service_api/{service_api}/edit",
 *     "delete-form" = "/admin/structure/service_api/{service_api}/delete"
 *   }
 * )
 */
class ServiceAPI extends ConfigEntityBase implements ServiceAPIInterface {
  /**
   * The Service api ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Service api label.
   *
   * @var string
   */
  protected $label;

  /**
   * The Service api endpoint.
   *
   * @var string
   */
  protected $endpoint;

  /**
   * The Service Provider for the api.
   *
   * @var string
   */
  protected $service_provider;

  /**
   * {@inheritdoc}
   */
  public function getEndpoint() {
    return $this->pluginDefinition['title'];
  }

  /**
   * {@inheritdoc}
   */
  public function getServiceProvider() {
    return $this->pluginDefinition['title'];
  }

}
