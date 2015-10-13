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
   *
   * @todo does this implement some interface that we're not documenting?
   */
  public function routes() {
    $endpoints = \Drupal::entityManager()->getStorage('service_endpoint')->loadMultiple();
    /** @var $manager \Drupal\services\ServiceDefinitionPluginManager */
    $manager = \Drupal::service('plugin.manager.services.service_definition');

    $routes = array();

    /** @var $endpoint \Drupal\services\ServiceEndpointInterface */
    foreach ($endpoints as $endpoint) {
      foreach ($endpoint->getServiceProviders() as $service_def) {
        $parameters = [];
        /** @var $plugin_definition \Drupal\services\ServiceDefinitionInterface */
        $plugin_definition = $manager->getDefinition($service_def);
        $instance_of_services_def = $manager->createInstance($service_def, []);

        /**
         * @var $context_id string
         * @var $context_definition \Drupal\Core\Plugin\Context\ContextDefinition
         */
        if (!empty($plugin_definition['context'])) {
          foreach ($plugin_definition['context'] as $context_id => $context_definition) {
            // Build an array of parameter to pass to the Route definitions.
            $parameters[$context_id] = [
              'type' => $context_definition->getDataType(),
            ];
          }
        }
        // Dynamically building custom routes per enabled plugin on an endpoint entity.
        $route = new Route(
          '/' . $endpoint->getEndpoint() . '/' . $plugin_definition['path'],
          array(
            '_controller' => '\Drupal\services\Controller\Services::processRequest',
            'service_endpoint_id' => $endpoint->id(),
            'service_definition_id' => $service_def
          ),
          [],
          [
            'parameters' => $parameters
          ],
          '',
          [],
          $plugin_definition['methods']
        );
        $instance_of_services_def->processRoute($route);
        $routes['services.endpoint.' . $endpoint->id() . '.' . $service_def] = $route;
      }
    }
    return $routes;
  }

}
