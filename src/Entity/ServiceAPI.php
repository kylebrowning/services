<?php

/**
 * @file
 * Contains Drupal\services\Entity\ServiceAPI.
 */

namespace Drupal\services\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Routing\RouteMatch;
use Drupal\services\ServiceAPIInterface;
use Symfony\Component\HttpFoundation\Request;

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
 *     "delete-form" = "/admin/structure/service_api/{service_api}/delete",
 *     "collection" = "/admin/structure/service_api"
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

}
