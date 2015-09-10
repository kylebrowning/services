<?php
/**
 * @file
 * Contains \Drupal\services\Routing\ServiceAPI.
 */

namespace Drupal\services\Routing;
use Symfony\Component\Routing\Route;
/**
 * Defines dynamic routes.
 */
class ServiceAPI {

  /**
   * {@inheritdoc}
   */
  public function routes() {
    $apis = \Drupal::entityManager()->getStorage('service_api')->loadMultiple();

    $routes = array();

    /** @var $api \Drupal\services\ServiceAPIInterface */
    foreach ($apis as $api) {
      /** @var $service_provider \Drupal\services\ServiceDefinitionInterface */
      foreach ($api->getServiceProviders() as $service_def) {
        $routes['services.api' . $api->id() . '.' . $service_def->getPluginId()] = new Route(
          '/' . $api->getEndpoint(),
          array(
            '_controller' => '\Drupal\services\Controller\Services::processRequest',
            'service_api_id' => $api->id(),
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
