<?php

/**
 * @file
 * Contains Drupal\services\Controller\Services.
 */

namespace Drupal\services\Controller;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * Class Services.
 *
 * @package Drupal\services\Controller
 */
class Services extends ControllerBase {

  /**
   * @var \Drupal\Component\Plugin\PluginManagerInterface
   */
  protected $serviceDefinitionManager;

  /**
   * @var \Drupal\Component\Plugin\PluginManagerInterface
   */
  protected $serviceResponseManager;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('plugin.manager.services.service_definition'), $container->get('plugin.manager.services.service_response'), $container->get('serializer'));
  }

  function __construct(PluginManagerInterface $service_definition_manager, PluginManagerInterface $service_response_manager, SerializerInterface $serializer) {
    $this->serviceDefinitionManager = $service_definition_manager;
    $this->serviceResponseManager = $service_response_manager;
    $this->serializer = $serializer;
  }


  /**
   * Processing the API request.
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, $service_endpoint_id, $service_definition_id) {
    /** @var $service_endpoint \Drupal\services\ServiceEndpointInterface */
    $service_endpoint = $this->entityManager()->getStorage('service_endpoint')->load($service_endpoint_id);

    //TODO - pull in settings from service API and alter response

    /** @var $service_def \Drupal\services\ServiceDefinitionInterface */
    $service_def = $this->serviceDefinitionManager->createInstance($service_definition_id, []);
    $content = $service_def->processRequest($request, $route_match);
    /** @var $responder \Drupal\services\ServiceResponseInterface */
    $responder = $this->serviceResponseManager->getInstance(['request' => $request]);
    return $responder->respond($content, $request, $this->serializer);
  }
}
