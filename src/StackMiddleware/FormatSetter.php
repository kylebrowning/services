<?php
/**
 * @file
 * Contains \Drupal\services\StackMiddleware\FormatSetter.php
 */

namespace Drupal\services\StackMiddleware;

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpKernel\HttpKernelInterface;

class FormatSetter implements HttpKernelInterface {

  /**
   * The wrapped HTTP kernel.
   *
   * @var \Symfony\Component\HttpKernel\HttpKernelInterface
   */
  protected $httpKernel;

  /**
   * Constructs a PageCache object.
   *
   * @param \Symfony\Component\HttpKernel\HttpKernelInterface $http_kernel
   *   The decorated kernel.
   */
  public function __construct(HttpKernelInterface $http_kernel) {
    $this->httpKernel = $http_kernel;
  }

  public function handle(Request $request, $type = self::MASTER_REQUEST, $catch = TRUE) {
    if ($request->headers->has('Accept')) {
      $request->setRequestFormat($request->getFormat($request->headers->get('Accept')));
    }
    return $this->httpKernel->handle($request, $type, $catch);
  }

}
