<?php

/**
 * @file
 * Contains Drupal\services\Form\ServiceAPIForm.
 */

namespace Drupal\services\Form;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityForm;
use Drupal\Core\Form\FormStateInterface;
use Drupal\services\ServiceDefinitionPluginManager;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Class ServiceAPIForm.
 *
 * @package Drupal\services\Form
 */
class ServiceAPIForm extends EntityForm {

  /**
   * @var \Drupal\Component\Plugin\PluginManagerInterface
   */
  protected $manager;

  public static function create(ContainerInterface $container) {
    return new static($container->get('plugin.manager.services.service_definition'));
  }

  function __construct(PluginManagerInterface $manager) {
    $this->manager = $manager;
  }
  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);

    /** @var $service_api \Drupal\services\Entity\ServiceAPI */
    $service_api = $this->entity;
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Label'),
      '#maxlength' => 255,
      '#default_value' => $service_api->label(),
      '#description' => $this->t("Label for the Service api."),
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#default_value' => $service_api->id(),
      '#machine_name' => array(
        'exists' => '\Drupal\services\Entity\ServiceAPI::load',
      ),
      '#disabled' => !$service_api->isNew(),
    );

    $form['endpoint'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Endpoint'),
      '#maxlength' => 255,
      '#default_value' => $service_api->getEndpoint(),
      '#description' => $this->t("URL endpoint."),
      '#required' => TRUE,
    );

    $opts = [];

    foreach ($this->manager->getDefinitions() as $plugin_id => $definition) {
      $opts[$plugin_id] = [
        t((string) $definition['title']),
        t((string) $definition['endpoint']),
        t((string) $definition['arguments'])
      ];
    }

    $form['service_providers'] = array(
      '#type' => 'tableselect',
      '#header' => [
        'title'=>t('Definition'),
        'endpoint'=>t('Endpoint'),
        'arguments'=>t('Arguments')
      ],
      '#options' => $opts,
      '#title' => $this->t('Service Provider'),
      '#empty' => t('No service definitions exist'),
      '#required' => TRUE,
      '#default_value' => $service_api->getServiceProviders(),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $service_api = $this->entity;
    $status = $service_api->save();

    if ($status) {
      drupal_set_message($this->t('Saved the %label Service api.', array(
        '%label' => $service_api->label(),
      )));
    }
    else {
      drupal_set_message($this->t('The %label Service api was not saved.', array(
        '%label' => $service_api->label(),
      )));
    }
    $form_state->setRedirectUrl($service_api->urlInfo('collection'));
  }

}
