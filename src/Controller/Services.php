<?php

/**
 * @file
 * Contains Drupal\services\Controller\Services.
 */

namespace Drupal\services\Controller;

use Drupal\Component\Plugin\PluginManagerInterface;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Plugin\Context\Context;
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
   * @var \Symfony\Component\Serializer\SerializerInterface
   */
  protected $serializer;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static($container->get('plugin.manager.services.service_definition'), $container->get('serializer'));
  }

  function __construct(PluginManagerInterface $service_definition_manager, SerializerInterface $serializer) {
    $this->serviceDefinitionManager = $service_definition_manager;
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
    foreach ($service_def->getContextDefinitions() as $context_id => $context_definition) {
      if ($request->attributes->has($context_id)) {
        $context = new Context($context_definition);
        $context->setContextValue($request->attributes->get($context_id));
        $service_def->setContext($context_id, $context);
      }
    }
    $content = $service_def->processRequest($request, $route_match);
    $accept = $request->headers->get('Accept');
    $format = $request->getFormat($accept);
    $data = $this->serializer->serialize($content, $format);
    $response = new Response($data);
    $response->headers->add(['Content-Type' => $accept]);
    return $response;
  }
}
