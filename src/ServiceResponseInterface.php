<?php
/**
 * @file
 * Contains \Drupal\service\ServiceResponseInterface.php
 */

namespace Drupal\services;


use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

interface ServiceResponseInterface {

  /**
   * @param $data
   * @param \Symfony\Component\HttpFoundation\Request $request
   * @param \Symfony\Component\Serializer\SerializerInterface $serializer
   *
   * @return \Symfony\Component\HttpFoundation\Response
   */
  public function respond($data, Request $request, SerializerInterface $serializer);

}