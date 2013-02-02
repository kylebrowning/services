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
  public function get($name = NULL) {
    if ($name) {
      // Core only supports routes with single /. In theory we could build
      // routes if we could discover every single config in system and move to
      // an approach similar to entities. This would then give :
      // services/config/config_name/{id} where id is the name of the config
      // item that is being updated. So the ability to affect multiple items
      // would be gone and this might be correct.
      $parameters = explode('::', $name);
      if ($config = config($parameters[0])) {
        $value = $config->get(!empty($parameters[1]) ? $parameters[1] : '');
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

    /**
   * Responds to config DELETE requests.
   *
   * @param mixed $name
   *   The entity ID.
   *
   * @return \Drupal\rest\ResourceResponse
   *   The HTTP response object.
   *
   * @throws \Symfony\Component\HttpKernel\Exception\HttpException
   */
  public function delete($id) {
    $parameters = explode('::', $name);
    if ($config = config($parameters[0])) {
      if (!empty($parameters[1])) {
        $config->clear($parameters[1]);
      }
      else {
        $config->delete();
      }
      $config->save();
      // Delete responses have an empty body.
      return new ResourceResponse(NULL, 204);
    }
    throw new NotFoundHttpException(t('Config with name @name not found', array('@name' => $name)));
  }
}
