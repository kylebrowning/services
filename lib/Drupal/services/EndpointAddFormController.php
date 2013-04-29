<?php

/**
 * @file
 * Contains Drupal\views_ui\ViewAddFormController.
 */

namespace Drupal\services;

use Drupal\Core\Entity\EntityFormController;
use Drupal\Core\Entity\EntityInterface;
use Drupal\services\Endpoint;

/**
 * Form controller for the Endpoint edit form.
 */
class EndpointAddFormController extends EntityFormController {

  /**
   * Overrides Drupal\Core\Entity\EntityFormController::prepareForm().
   */
  protected function prepareEntity(EntityInterface $view) {
    // Do not prepare the entity while it is being added.
  }

  /**
   * Overrides Drupal\Core\Entity\EntityFormController::form().
   */
  public function form(array $form, array &$form_state, EntityInterface $endpoint) {

    $form['name'] = array(
      '#type' => 'fieldset',
      '#attributes' => array('class' => array('fieldset-no-legend')),
    );

    $form['name']['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Endpoint name'),
      '#required' => TRUE,
      '#size' => 32,
      '#default_value' => '',
      '#maxlength' => 255,
    );

    $form['name']['id'] = array(
      '#type' => 'machine_name',
      '#maxlength' => 128,
      '#machine_name' => array(
        'exists' => 'services_get_endpoint',
        'source' => array('name', 'label'),
      ),
      '#description' => t('A unique machine-readable name for this Endpoint. It must only contain lowercase letters, numbers, and underscores.'),
    );

    $form['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Endpoints URI path'),
      '#required' => TRUE,
      '#size' => 32,
      '#default_value' => '',
      '#maxlength' => 255,
    );

    return $form;
  }

  /**
   * Overrides Drupal\Core\Entity\EntityFormController::actions().
   */
  protected function actions(array $form, array &$form_state) {
    $actions = parent::actions($form, $form_state);
    $actions['submit']['#value'] = t('Save');

    $actions['cancel'] = array(
      '#value' => t('Cancel'),
      '#submit' => array(
        array($this, 'cancel'),
      ),
      '#limit_validation_errors' => array(),
    );
    return $actions;
  }

  /**
   * Overrides Drupal\Core\Entity\EntityFormController::validate().
   */
  public function validate(array $form, array &$form_state) {
  }

  /**
   * Overrides Drupal\Core\Entity\EntityFormController::submit().
   */
  public function submit(array $form, array &$form_state) {
    $fv = $form_state['values'];
    $endpoint_form_values = array(
      'label' => $fv['label'],
      'id' => $fv['id'],
      'path' => $fv['path'],
    );

    $endpoint = $this->entityManager->getStorageController('endpoint')->create($endpoint_form_values);
    $endpoint->save();

    $form_state['redirect'] = array('admin/structure/services');
//    $form_state['redirect'] = array('admin/structure/services/endpoint/' . $endpoint->id());
  }

  /**
   * Form submission handler for the 'cancel' action.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param array $form_state
   *   A reference to a keyed array containing the current state of the form.
   */
  public function cancel(array $form, array &$form_state) {
    $form_state['redirect'] = 'admin/structure/services';
  }

}
