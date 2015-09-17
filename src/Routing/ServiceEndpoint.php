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
    /** @var $manager \Drupal\services\ServiceDefinitionPluginManager */
    $manager = \Drupal::service('plugin.manager.services.service_definition');

    $routes = array();

    /** @var $endpoint \Drupal\services\ServiceEndpointInterface */
    foreach ($endpoints as $endpoint) {
      /** @var $service_provider \Drupal\services\ServiceDefinitionInterface */
      foreach ($endpoint->getServiceProviders() as $service_def) {
        /** @var $plugin_definition \Drupal\services\ServiceDefinitionInterface */
        $plugin_definition = $manager->getDefinition($service_def);
        /**
         * @var $context_id string
         * @var $context_definition \Drupal\Core\Plugin\Context\ContextDefinition
         */
        foreach ($plugin_definition['context'] as $context_id => $context_definition) {
          $contexts[$context_id] = [
            'type' => $context_definition->getDataType(),
            'converter' => "paramconverter.entity"
          ];
        }
        $routes['services.endpoint.' . $endpoint->id() . '.' . $service_def] = new Route(
          '/' . $endpoint->getEndpoint() . '/' . $plugin_definition['path'],
          array(
            '_controller' => '\Drupal\services\Controller\Services::processRequest',
            'service_endpoint_id' => $endpoint->id(),
            'service_definition_id' => $service_def
          ),
          array(
            '_access' => 'TRUE',
          ),
          [
            'compiler_class' => '\Drupal\Core\Routing\RouteCompiler',
            'parameters' => $contexts
          ],
          '',
          [],
          $plugin_definition['methods']
        );
      }
    }
    return $routes;
  }

}
