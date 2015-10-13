<?php

/**
 * @file
 * Contains Drupal\services\Entity\ServiceEndpoint.
 */

namespace Drupal\services\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\services\ServiceEndpointInterface;

/**
 * Defines the service endpoint entity.
 *
 * @ConfigEntityType(
 *   id = "service_endpoint",
 *   label = @Translation("service endpoint"),
 *   handlers = {
 *     "list_builder" = "Drupal\services\Controller\ServiceEndpointListBuilder",
 *     "form" = {
 *       "add" = "Drupal\services\Form\ServiceEndpointForm",
 *       "edit" = "Drupal\services\Form\ServiceEndpointForm",
 *       "delete" = "Drupal\services\Form\ServiceEndpointDeleteForm"
 *     }
 *   },
 *   config_prefix = "service_endpoint",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/service_endpoint/{service_endpoint}",
 *     "edit-form" = "/admin/structure/service_endpoint/{service_endpoint}/edit",
 *     "delete-form" = "/admin/structure/service_endpoint/{service_endpoint}/delete",
 *     "collection" = "/admin/structure/service_endpoint"
 *   }
 * )
 */
class ServiceEndpoint extends ConfigEntityBase implements ServiceEndpointInterface {
  /**
   * The services endpoint ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The services endpoint label.
   *
   * @var string
   */
  protected $label;

  /**
   * The services endpoint.
   *
   * @var string
   */
  protected $endpoint;

  /**
   * The service providers for the api.
   *
   * @var string
   */
  protected $service_providers;

  /**
   * {@inheritdoc}
   */
  public function getEndpoint() {
    return $this->endpoint;
  }

  /**
   * {@inheritdoc}
   */
  public function getServiceProviders() {
    return $this->service_providers;
  }

  public function postSave(EntityStorageInterface $storage, $update = TRUE) {
    parent::postSave($storage, $update);
    \Drupal::service('router.builder')->setRebuildNeeded();
  }

  public static function postDelete(EntityStorageInterface $storage, array $entities) {
    parent::postDelete($storage, $entities);
    \Drupal::service('router.builder')->setRebuildNeeded();
  }

}
