<?php

/**
 * Definition of \Drupal\services\EventSubscriber\RouteSubscriber.
 */

namespace Drupal\services\EventSubscriber;

use Drupal\Core\Config\ConfigFactory;
use Drupal\Core\Routing\RouteBuildEvent;
use Drupal\Core\Routing\RoutingEvents;
use Drupal\rest\Plugin\Type\ResourcePluginManager;

use Symfony\Component\EventDispatcher\EventSubscriberInterface;

/**
 * Subscriber for services-style routes.
 */
class RouteSubscriber implements EventSubscriberInterface {

  /**
   * The plugin manager for services plugins.
   *
   * @var \Drupal\rest\Plugin\Type\ResourcePluginManager
   */
  protected $manager;

  /**
   * The Drupal configuration factory.
   *
   * @var \Drupal\Core\Config\ConfigFactory
   */
  protected $config;

  /**
   * Constructs a RouteSubscriber object.
   *
   * @param \Drupal\rest\Plugin\Type\ResourcePluginManager $manager
   *   The resource plugin manager.
   * @param \Drupal\Core\Config\ConfigFactory $config
   *   The configuration factory holding resource settings.
   */
  public function __construct(ResourcePluginManager $manager, ConfigFactory $config) {
    $this->manager = $manager;
    $this->config = $config;
  }

  /**
   * Adds routes to enabled REST resources.
   *
   * @param \Drupal\Core\Routing\RouteBuildEvent $event
   *   The route building event.
   */
  public function dynamicRoutes(RouteBuildEvent $event) {

    $collection = $event->getRouteCollection();
    $enabled_resources = $this->config->get('rest.settings')->load()->get('resources');

    // Iterate over all enabled resource plugins.
    foreach ($enabled_resources as $id => $enabled_methods) {
      $plugin = $this->manager->getInstance(array('id' => $id));

      foreach ($plugin->routes() as $route) {
        $operation = $route->getRequirement('_operation');
        if ($operation) {
          $collection->add("services.$operation", $route);
        }
      }
    }
  }

  /**
   * Implements EventSubscriberInterface::getSubscribedEvents().
   */
  static function getSubscribedEvents() {
    $events[RoutingEvents::DYNAMIC] = 'dynamicRoutes';
    return $events;
  }
}

