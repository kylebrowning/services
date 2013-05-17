<?php

/**
 * @file
 * Definition of Drupal\rest\Plugin\ResourceBase.
 */

namespace Drupal\services\Plugin;

use Drupal\rest\Plugin\ResourceBase;
use Symfony\Component\Routing\Route;
use Symfony\Component\Routing\RouteCollection;
use Doctrine\Common\Annotations\AnnotationReader;
use Doctrine\Common\Annotations\AnnotationRegistry;

/**
 * Common base class for resource plugins.
 */
abstract class AnnotationResourceBase extends ResourceBase {

  /**
   * Method annotations.
   *
   * @var array
   */
  protected $annotations = array();

  /**
   * @var AnnotationReader.
   */
  protected $reader;

  public function __construct(array $configuration, $plugin_id, array $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->reader = new AnnotationReader();

    $methods = $this->getMethodNames();

    $this->registerAnnotationAutoloaderNamespace();

    foreach ($methods as $method_name) {
      $annotation_definition = $this->getResourceMethodAnnotationDefinition($method_name);

      if (!empty($annotation_definition )) {
        $this->annotations[$method_name] = $annotation_definition;
      }
    }
  }

  /**
   * Implements ResourceInterface::routes().
   */
  public function routes() {
    $collection = parent::routes();
    $path_prefix = strtr($this->pluginId, ':', '/');
    $route_name = strtr($this->pluginId, ':', '.');

    $methods = $this->getAnnotatedMethods();
    foreach ($methods as $method) {
      $lower_method = strtolower($method);
      $annotation = $this->getMethodAnnotation($method);

      $route = new Route('/' . $annotation['uri'], array(
        '_controller' => 'Drupal\services\AnnotationRequestHandler::handle',
        // Pass the resource plugin ID along as default property.
        '_plugin' => $this->pluginId,
      ), array(
        // The HTTP method is a requirement for this route.
        '_method' => $annotation['httpMethod'],
        '_operation' => $method,
        '_permission' => 'access content',
      ));

      $collection->add("$route_name.$lower_method", $route);
    }

    return $collection;
  }

  /**
   * We will use annotated methods for building routes.
   */
  public function getAnnotatedMethods() {
    return array_keys($this->annotations);
  }

  /**
   * Retrieve annotation for specified method.
   *
   * @param string $method_name
   *
   * @return array
   */
  public function getMethodAnnotation($method_name) {
    return isset($this->annotations[$method_name]) ? $this->annotations[$method_name] : NULL;
  }

  protected function getMethodNames() {
    $reflection = new \ReflectionClass(get_class($this));

    $methods = array();
    foreach ($reflection->getMethods() as $reflection_method) {
      $methods[] = $reflection_method->name;
    }

    return $methods;
  }

  protected function registerAnnotationAutoloaderNamespace() {
    $annotation_namespaces = array(
      'Drupal\services\Annotation' => DRUPAL_ROOT . '/modules/services/lib',
    );
    AnnotationRegistry::registerAutoloadNamespaces($annotation_namespaces);
  }

  protected function getResourceMethodAnnotationDefinition($method_name) {
    $reflection_method = new \ReflectionMethod(get_class($this), $method_name);
    $annotation = $this->reader->getMethodAnnotation($reflection_method, 'Drupal\services\Annotation\ResourceMethod');
    if (!empty($annotation)) {
      return $annotation->get();
    }
  }
}
