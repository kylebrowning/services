<?php
/**
 * @file
 * Contains \Drupal\service\ServiceResponseBase.php
 */

namespace Drupal\services;


use Drupal\Core\Plugin\PluginBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\Response;
use Symfony\Component\Serializer\SerializerInterface;

abstract class ServiceResponseBase extends PluginBase implements ServiceResponseInterface {

  public function respond($data, Request $request, SerializerInterface $serializer) {
    $data = $serializer->serialize($data, $this->getPluginId());
    $response = new Response($data);
    $response->headers->add(['Content-Type' => $request->headers->get('Accept')]);
    return $response;
  }

}
