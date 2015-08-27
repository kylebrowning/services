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
    $apis = \Drupal::entityManager()->getStorage('service_endpoint')->loadMultiple();

    $routes = array();

    /** @var $api \Drupal\services\ServiceEndpointInterface */
    foreach ($apis as $api) {
      $routes['services.' . $api->id()] = new Route(
        '/' . $api->getEndpoint(),
        array(
          '_controller' => '\Drupal\services\Controller\Services::processRequest',
        ),
        array(
          '_access' => 'TRUE',
        )
      );
    }
    return $routes;
  }

}
