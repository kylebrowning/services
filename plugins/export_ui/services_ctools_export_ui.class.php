<?php

/**
 * @file
 * Export-ui handler for the Services module.
 */

class services_ctools_export_ui extends ctools_export_ui {

  /**
   * Page callback for the resources page.
   */
  function resources_page($js, $input, $item) {
    drupal_set_title($this->get_page_title('resources', $item));
    return services_edit_endpoint_resources($item);
  }

  /**
   * Page callback for the authentication page.
   */
  function authentication_page($js, $input, $item) {
    drupal_set_title($this->get_page_title('authentication', $item));
    return drupal_get_form('services_edit_form_endpoint_authentication', $item);
  }
}

/**
 * Endpoint authentication configuration form.
 */
function services_edit_form_endpoint_authentication($form, &$form_state) {
  list($endpoint) = $form_state['build_info']['args'];
  // Loading runtime include as needed by services_authentication_info().
  module_load_include('runtime.inc', 'services');

  $auth_modules = module_implements('services_authentication_info');

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  if (empty($auth_modules)) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('No installed authentication modules'),
      '#description'   => t('No authentication modules are installed, standard ' .
        'Drupal session based security will be used.'),
    );
  }
  elseif (empty($endpoint->authentication)) {
    $form['message'] = array(
      '#type'          => 'item',
      '#title'         => t('No enabled authentication modules'),
      '#value'   => t('No authentication modules are enabled, standard ' .
        'Drupal session based security will be used.'),
    );
  }
  else {
    // Add configuration fieldsets for the authentication modules
    foreach ($endpoint->authentication as $module => $settings) {
      $info = services_authentication_info($module);
      if ($info) {
        $form[$module] = array(
          '#type' => 'fieldset',
          '#title' => $info['title'],
          '#tree' => TRUE,
        ) + services_auth_invoke($module, 'security_settings', $settings);
      }
    }
  }

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

function services_edit_form_endpoint_authentication_submit($form, $form_state) {
  $endpoint = $form_state['values']['endpoint_object'];

  foreach (array_keys($endpoint->authentication) as $module) {
    $endpoint->authentication[$module] = $form_state['values'][$module];
  }

  drupal_set_message(t('Your authentication options have been saved.'));
  services_endpoint_save($endpoint);
}


/**
 * services_edit_endpoint_resources function.
 *
 * Edit Resources endpoint form
 * @param object $endpoint
 * @return string  The form to be displayed
 */
function services_edit_endpoint_resources($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  if ($endpoint && !empty($endpoint->title)) {
    drupal_set_title(check_plain($endpoint->title));
  }
  return drupal_get_form('services_edit_form_endpoint_resources', $endpoint);
}

/**
 * services_edit_form_endpoint_resources function.
 *
 * @param array &$form_state
 * @param object $endpoint
 * @return Form
 */
function services_edit_form_endpoint_resources(&$form_state, $endpoint) {
  module_load_include('resource_build.inc', 'services');

  $form = array();

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  $ops = array(
    'create'   => t('Create'),
    'retrieve' => t('Retrieve'),
    'update'   => t('Update'),
    'delete'   => t('Delete'),
    'index'    => t('Index'),
  );

  // Call _services_build_resources() directly instead of
  // services_get_resources to bypass caching.
  $resources = _services_build_resources();
  // Apply the endpoint in a non-strict mode, so that the non-active resources
  // are preserved.
  _services_apply_endpoint($resources, $endpoint, FALSE);

  $res = array(
    '#tree' => TRUE,
  );

  foreach ($resources as $name => $resource) {
    $rc = $resource['endpoint'];
    $res_set = array(
      '#type'        => 'fieldset',
      '#title'       => t('!name resource', array(
        '!name' => preg_replace('/[_-]+/', ' ', $name),
      )),
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
      '#tree'        => TRUE,
      '#attributes'  => array(
        'class' => array('resource'),
      ),
    );

    $res_set['alias'] = array(
      '#type'          => 'textfield',
      '#title'         => t('Alias'),
      '#description'   => t('The alias you enter here will be used instead of the resource name.'),
      '#size'          => 40,
      '#maxlength'     => 255,
      '#default_value' => isset($rc['alias']) ? $rc['alias'] : '',
    );

    $res_set['operations'] = array(
      '#tree' => TRUE,
    );
    foreach ($ops as $op => $title) {
      if (isset($resource[$op])) {
        $res_set['operations'][$op] = array(
          '#type'        => 'fieldset',
          '#title'       => $title,
          '#collapsible' => TRUE,
          '#collapsed'   => FALSE,
        );
        _services_resource_operation_settings($res_set['operations'][$op], $endpoint, $resource, $op);
      }
    }

    $classes = array(
      'actions'          => 'actions',
      'targeted_actions' => 'targeted actions',
      'relationships'    => 'relationships',
    );
    foreach ($classes as $element => $class) {
      if (!empty($resource[$class])) {
        $res_set[$element] = array(
          '#type'  => 'fieldset',
          '#title' => t($class),
          '#tree'  => TRUE,
        );
        foreach ($resource[$class] as $action => $definition) {
          $res_set[$element][$action] = array(
            '#type'        => 'fieldset',
            '#title'       => $action,
            '#collapsible' => TRUE,
            '#collapsed'   => FALSE,
          );
          _services_resource_operation_settings($res_set[$element][$action], $endpoint, $resource, $class, $action);
        }
      }
    }

    drupal_alter('services_resource_settings', $res_set, $resource);

    $res[$name] = $res_set;
  }

  $form['resources'] = $res;

  $form['save'] = array(
    '#type'  => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * services_edit_form_endpoint_resources_validate function.
 *
 * @param array $form
 * @param array $form_state
 * @return void
 */
function services_edit_form_endpoint_resources_validate($form, $form_state) {
  $res = $form_state['values']['resources'];
  // Validate aliases
  foreach ($res as $name => $resource) {
    if (!empty($resource['alias'])) {
      if (!preg_match('/^[a-z-]+$/', $resource['alias'])) {
        form_set_error("resources][{$name}][alias", t("The alias for the !name may only contain lower case a-z and dashes.", array(
          '!name' => $form['resources'][$name]['#title'],
        )));
      }
    }
  }
}

/**
 * services_edit_form_endpoint_resources_submit function.
 *
 * @param array $form
 * @param array $form_state
 * @return void
 */
function services_edit_form_endpoint_resources_submit($form, $form_state) {
  $resources = $form_state['input']['resources'];
  $endpoint  = $form_state['build_info']['args'][0];
  //Loop through submitted resources and build the final resource for submission
  foreach ($resources as $name => $resource) {
    $used = FALSE;
    $final_resource = isset($endpoint->resources[$name]) ? $endpoint->resources[$name] : array();
    //Add the alias that was submitted
    $final_resource['alias'] = $resource['alias'];
    //Check if operations were submitted
    if (isset($resource['operations'])) {
      //Loop through operations that were submitted and add them to our final resource structure
      foreach ($resource['operations'] as $op => $def) {
        $cop = isset($final_resource['operations'][$op]) ? $final_resource['operations'][$op] : array();
        //take old value and overwrite it
        $cop = array_merge($cop, $def);
        //If were still enabled set the operation method to enabled
        if ($cop['enabled']) {
          $final_resource['operations'][$op] = $cop;
          $used = $used || TRUE;
        }
        //Since it wasnt used, IE, null unset this value
        else {
          unset($final_resource['operations'][$op]);
        }
      }
    }
    $classes = array(
      'actions' => 'actions',
      'targeted_actions' => 'targeted actions',
      'relationships' => 'relationships',
    );
    foreach ($classes as $element => $class) {
      $class_used = FALSE;
      if (!empty($resource[$element])) {
        foreach ($resource[$element] as $act => $def) {
          $cop = isset($final_resource[$class][$act]) ? $final_resource[$class][$act] : array();
          $cop = array_merge($cop, $def);
          if ($cop['enabled']) {
            $final_resource[$class][$act] = $cop;
            $class_used = $class_used || TRUE;
          }
          else {
            unset($final_resource[$class][$act]);
          }
        }
        if (!$class_used) {
          unset($final_resource[$class]);
        }
        $used = $class_used || $used;
      }
    }

    if ($used) {
      $endpoint->resources[$name] = $final_resource;
    }
    else {
      unset($endpoint->resources[$name]);
    }
  }
  drupal_set_message(t('Your resources have been saved.'));
  services_endpoint_save($endpoint);
}

/**
 * Returns information about a resource operation given it's class and name.
 *
 * @return array
 *  Information about the operation, or NULL if no matching
 *  operation was found.
 */
function services_get_resource_operation_info($resource, $class, $name = NULL) {
  $op = NULL;

  if (isset($resource[$class])) {
    $op = $resource[$class];
    if (!empty($name)) {
      $op = isset($op[$name]) ? $op[$name] : NULL;
    }
  }

  return $op;
}
/**
 * Constructs the settings form for resource operation.
 *
 * @param array $settings
 *  The root element for the settings form.
 * @param string $resource
 *  The resource information array.
 * @param string $class
 *  The class of the operation. Can be 'create', 'retrieve', 'update',
 *  'delete', 'index', 'actions' or 'targeted actions' or 'relationships'.
 * @param string $name
 *  Optional. The name parameter is only used for actions, targeted actions
 *  and relationship.
 */
function _services_resource_operation_settings(&$settings, $endpoint, $resource, $class, $name = NULL) {
  module_load_include('runtime.inc', 'services');
  if ($rop = services_get_resource_operation_info($resource, $class, $name)) {
    $settings['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => !empty($rop['endpoint']) && $rop['endpoint']['enabled'],
    );

    if (!empty($rop['endpoint']['preprocess'])) {
      $settings['preprocess'] = array(
        '#type' => 'item',
        '#title' => t('Preprocess function'),
        '#value' => $rop['endpoint']['preprocess'],
      );
    }

    if (!empty($rop['endpoint']['postprocess'])) {
      $settings['preprocess'] = array(
        '#type' => 'item',
        '#title' => t('Postprocess function'),
        '#value' => $rop['endpoint']['Postprocess'],
      );
    }

    // Let authentication modules add their configuration options
    if(isset($endpoint->authentication))
    foreach ($endpoint->authentication as $auth_module => $auth_settings) {
      $settings_form = services_auth_invoke($auth_module, 'controller_settings', $auth_settings, $rop, $endpoint, $class, $name);
      if (!empty($settings_form)) {
        $settings[$auth_module] = $settings_form;
      }
    }

    drupal_alter('services_resource_operation_settings', $settings, $endpoint, $resource, $class, $name);
  }
}
