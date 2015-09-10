<?php
/**
 * @file
 * Provides Drupal\services\ServiceDefinitionBase.
 */

namespace Drupal\services;

use Drupal\Component\Plugin\PluginBase;
use Symfony\Component\HttpFoundation\Request;

abstract class ServiceDefinitionBase extends PluginBase implements ServiceDefinitionInterface {

  /**
   * {@inheritdoc}
   */
  public function getTitle() {
    return $this->pluginDefinition['title'];
  }

  /**
   * {@inheritdoc}
   */
  public function getCategory() {
    return $this->pluginDefinition['category'];
  }

  /**
   * {@inheritdoc}
   */
  public function getPath() {
    return $this->pluginDefinition['path'];
  }

  /**
   * {@inheritdoc}
   */
  public function getDescription() {
    return $this->pluginDefinition['description'];
  }

  /**
   * {@inheritdoc}
   */
  public function supportsTranslation() {
    return $this->pluginDefinition['translatable'];
  }

  /**
   * {@inheritdoc}
   */
  public function getArguments() {
    return $this->pluginDefinition['arguments'];
  }
}
