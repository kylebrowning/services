<?php

/**
 * @file
 * Contains Drupal\services\Entity\ServiceEndpoint.
 */

namespace Drupal\services\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\services\ServiceEndpointInterface;
use Symfony\Component\HttpFoundation\Request;

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
   * The service endpoint ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The service endpoint label.
   *
   * @var string
   */
  protected $label;

  /**
   * The service endpoint endpoint.
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
    return $this->endpoint;
  }

  /**
   * {@inheritdoc}
   */
  public function getServiceProvider() {
    return $this->service_provider;
  }

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request) {
    if ($this->getServiceProvider()) {
      $plugin_service = \Drupal::getContainer()->get('plugin.manager.services.service_definition');
      /** @var $instance \Drupal\services\ServiceEndpointInterface */
      $instance = $plugin_service->createInstance($this->getServiceProvider());
      return $instance->processRequest($request);
    }
    return NULL;
  }
}
