<?php
/**
 * @file
 * Contains \Drupal\services\Routing\ServiceEndpoint.
 */

namespace Drupal\services\Routing;
use Symfony\Component\Routing\Route;
/**
 * Defines dynamic routes.
 */
class ServiceEndpoint {

  /**
   * {@inheritdoc}
   */
  public function routes() {
    $endpoints = \Drupal::entityManager()->getStorage('service_endpoint')->loadMultiple();

    $routes = array();

    /** @var $endpoint \Drupal\services\ServiceEndpointInterface */
    foreach ($endpoints as $endpoint) {
      /** @var $service_provider \Drupal\services\ServiceDefinitionInterface */
      foreach ($endpoint->getServiceProviders() as $service_def) {
        $routes['services.endpoint' . $endpoint->id() . '.' . $service_def->getPluginId()] = new Route(
          '/' . $endpoint->getEndpoint(),
          array(
            '_controller' => '\Drupal\services\Controller\Services::processRequest',
            'service_endpoint_id' => $endpoint->id(),
            'service_definition_id' => $service_def->getPluginId()
          ),
          array(
            '_access' => 'TRUE',
          )
        );
      }
    }
    return $routes;
  }

}
