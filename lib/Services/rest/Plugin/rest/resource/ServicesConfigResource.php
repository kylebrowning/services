<?php

/**
 * @file
 * Definition of Drupal\services\Plugin\rest\resource\ServicesConfigResource.
 */

namespace Drupal\services\Plugin\rest\resource;

use Drupal\Core\Annotation\Plugin;
use Drupal\Core\Annotation\Translation;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;

use Symfony\Component\HttpKernel\Exception\NotFoundHttpException;

/**
 * Provides a resources for config.
 *
 * @Plugin(
 *   id = "servicesconfig",
 *   label = @Translation("Configuration management")
 * )
 */
class ServicesConfigResource extends ResourceBase {

  /**
   * Responds to GET requests.
   *
   * Returns a configration specified key.
   *
   * @return \Drupal\rest\ResourceResponse
   *   The response containing the key value.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\HttpException
   */
  public function get($name = NULL, $key = '') {
    if ($name) {
      if ($config = config($name)) {
        $value = $config->get($key);
        if (!empty($value)) {
        // Serialization is done here, so we indicate with NULL that there is no
        // subsequent serialization necessary.
        $response = new ResourceResponse(NULL, 200, array('Content-Type' => 'application/vnd.drupal.ld+json'));
        // @todo remove hard coded format here.
        $response->setContent(drupal_json_encode($value));
        return $response;
      }
      }
    }
    throw new NotFoundHttpException('Not Found');
  }
}
