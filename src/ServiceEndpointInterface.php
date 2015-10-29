<?php

/**
 * @file
 * Contains Drupal\services\ServiceEndpointInterface.
 */

namespace Drupal\services;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\Core\Config\Entity\ConfigEntityInterface;
use Drupal\Core\Routing\RouteMatch;
use Symfony\Component\HttpFoundation\Request;

/**
 * Provides an interface for defining service endpoint entities.
 */
interface ServiceEndpointInterface extends ConfigEntityInterface {

  /**
   * Returns the endpoint path to the API.
   * @return string
   */
  public function getEndpoint();

  /**
   * Returns the service provider IDs.
   *
   * This is an array of plugin ids for ServiceDefinition plugins.
   *
   * @todo This should be expanded to include a PluginCollection on this entity
   * so that individual plugins can be instantiated and returned. As part of
   * this change, configurable plugins should be introduced and the
   * corresponding entity updates will need to be made.
   *
   * @return string[]
   */
  public function getServiceProviders();
}
