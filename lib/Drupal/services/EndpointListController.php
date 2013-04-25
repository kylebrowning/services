<?php

/**
 * @file
 * Definition of Drupal\services\EndpointListController.
 */

namespace Drupal\services;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Config\Entity\ConfigEntityListController;

/**
 * Provides a listing of Endpoints.
 */
class EndpointListController extends ConfigEntityListController {

  /**
   * Overrides Drupal\Core\Entity\EntityListController::buildRow();
   */
  public function buildRow(EntityInterface $endpoint) {
    return array(
      'data' => array(
        'name' => $endpoint->get('label'),
        'description' => $endpoint->get('description'),
        'path' => $endpoint->get('path'),
        'operations' => array(
          'data' => $this->buildOperations($endpoint),
        ),
      ),
      'title' => t('Machine name: ') . $endpoint->id(),
    );
  }

  /**
   * Overrides Drupal\Core\Entity\EntityListController::buildHeader();
   */
  public function buildHeader() {
    return array(
      'name' => array(
        'data' => t('Endpoint name'),
      ),
      'description' => array(
        'data' => t('Description'),
      ),
      'path' => array(
        'data' => t('Path'),
      ),
      'operations' => array(
        'data' => t('Operations'),
      ),
    );
  }

  /**
   * Overrides Drupal\Core\Entity\EntityListController::buildOperations();
   */
  public function buildOperations(EntityInterface $entity) {
    $build = parent::buildOperations($entity);

    // Use the dropbutton #type.
    unset($build['#theme']);
    $build['#type'] = 'dropbutton';

    return $build;
  }

  /**
   * Overrides Drupal\Core\Entity\EntityListController::render();
   */
  public function render() {
    $entities = $this->load();
    $list['#type'] = 'container';
    $list['#attributes']['id'] = 'endpoint-entity-list';
    $list['table'] = array(
      '#theme' => 'table',
      '#header' => $this->buildHeader(),
      '#rows' => array(),
      '#empty' => t('There are no endpoints.'),
    );
    foreach ($entities as $entity) {
      $list['table']['#rows'][$entity->id()] = $this->buildRow($entity);
    }

    return $list;
  }

}
