<?php

/**
 * @file
 * Contains \Drupal\services\ServiceManager.
 */

namespace Drupal\services;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Entity\EntityManagerInterface;

/**
 * Responsible for the services management service.
 */
class ServiceManager {

  /**
   * Create the ServicesManager.
   *
   * @param \Drupal\Core\Entity\EntityManagerInterface $entityManager
   *   The entity manager.
   * @param \Drupal\Component\Plugin\PluginManagerInterface $pluginManager
   *   The plugin manager.
   */
  public function __construct(EntityManagerInterface $entityManager, PluginManagerInterface $pluginManager) {
    $this->entityManager = $entityManager;
    $this->pluginManager = $pluginManager;
  }

  /*
  /*
   * Loading keys that are of the specified key provider.
   *
   * @param string $key_provider
   *   The key provider ID to use.
   */
  public function getServiceByEndpoint($endpoint) {

    // Check if the request endpoint starts or ends with '/', remove them.
    $endpoint = trim($endpoint, '/');

    return array_shift($this->entityManager->getStorage('service_endpoint')->loadByProperties(array('endpoint' => $endpoint)));
  }

}
