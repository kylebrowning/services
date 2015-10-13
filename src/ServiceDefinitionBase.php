<?php
/**
 * @file
 * Provides Drupal\services\ServiceDefinitionBase.
 */

namespace Drupal\services;


use Drupal\Core\Plugin\ContextAwarePluginBase;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Serializer\SerializerInterface;
use Symfony\Component\HttpKernel\Exception\HttpException;
use Symfony\Component\Routing\Route;

abstract class ServiceDefinitionBase extends ContextAwarePluginBase implements ServiceDefinitionInterface {

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

  /**
   * {@inheritdoc}
   */
  public function processRoute(Route $route) {}

  /**
   * {@inheritdoc}
   */
  public function checkAccess(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {}

  /**
   * {@inheritdoc}
   */
  public function processResponse(Response $response) {}


}
