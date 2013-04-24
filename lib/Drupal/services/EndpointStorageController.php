<?php

/**
 * @file
 * Contains \Drupal\services\EndpointStorageController.
 */

namespace Drupal\services;

use Drupal;
use Drupal\Core\Config\Entity\ConfigStorageController;
use Drupal\Core\Config\Config;
use Drupal\Core\Entity\EntityInterface;

/**
 * Defines a controller class for endpoints.
 */
class EndpointStorageController extends ConfigStorageController {

  /**
   * Overrides \Drupal\Core\Config\Entity\ConfigStorageController::postSave().
   */
  protected function postSave(EntityInterface $entity, $update) {
    // Rebuild routing cache.
    Drupal::service('router.builder')->rebuild();
  }

  /**
   * Overrides \Drupal\Core\Config\Entity\ConfigStorageController::postDelete().
   */
  protected function postDelete($entities) {
    // Rebuild routing cache.
    Drupal::service('router.builder')->rebuild();
  }

}
