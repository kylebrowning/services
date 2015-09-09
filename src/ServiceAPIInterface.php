<?php

/**
 * @file
 * Contains Drupal\services\ServiceAPIInterface.
 */

namespace Drupal\services;

use Drupal\Component\Serialization\SerializationInterface;
use Drupal\Core\Config\Entity\ConfigEntityInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Provides an interface for defining Service api entities.
 */
interface ServiceAPIInterface extends ConfigEntityInterface {

  /**
   * Returns the endpoint path to the API.
   * @return string
   */
  public function getEndpoint();

  /**
   * Returns the service provider ID.
   * @return \Drupal\services\ServiceDefinitionInterface[]
   */
  public function getServiceProviders();

  /**
   * Processes the service API request.
   * @return SerializationInterface
   */
  public function processRequest(Request $request);
}
