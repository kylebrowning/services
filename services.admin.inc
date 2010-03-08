<?php
// $Id$

/**
 * @file
 * Administrative functions for the Services module.
 *
 * This provides the UI to list, create, edit and delete endpoints.
 */

/**
 * Output a list of endpoints.
 */
function services_list_endpoint($js = NULL) {
  // Loading runtime include as needed by services_auth_info().
  module_load_include('runtime.inc', 'services');

  $header = array(
    array('data' => t('Title'),          'class' => 'services-endpoint-title'),
    array('data' => t('Path'),           'class' => 'services-endpoint-path'),
    array('data' => t('Server'),         'class' => 'services-endpoint-server'),
    array('data' => t('Authentication'), 'class' => 'services-endpoint-authentication'),
    array('data' => t('Storage'),        'class' => 'services-endpoint-storage'),
    array('data' => t('Operations'),     'class' => 'services-endpoint-operations'),
  );

  $endpoints = services_endpoint_load_all();
  $rows = array();

  foreach ($endpoints as $endpoint) {
    $operations = array();

    if (empty($endpoint->disabled)) {
      $operations[] = array(
        'title' => t('Edit'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/edit',
      );
      $operations[] = array(
        'title' => t('Export'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/export',
      );
    }

    if ($endpoint->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $operations[] = array(
        'title' => t('Revert'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/delete',
      );
    }
    elseif ($endpoint->export_type != EXPORT_IN_CODE) {
      $operations[] = array(
        'title' => t('Delete'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/delete',
      );
    }
    elseif (empty($endpoint->disabled)) {
      $operations[] = array(
        'title' => t('Disable'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/disable',
        'query' => drupal_get_destination(),
      );
    }
    else {
      $operations[] = array(
        'title' => t('Enable'),
        'href'  => 'admin/build/services/' . $endpoint->name . '/enable',
        'query' => drupal_get_destination(),
      );
    }

    $auth_names = array();
    foreach (array_keys($endpoint->authentication) as $module) {
      $info = services_authentication_info($module);
      $auth_names[] = $info['#title'];
    }

    $rows[$endpoint->name] = array(
      'data' => array(
        'title' => array(
          'data'  => $endpoint->title,
          'class' => 'services-endpoint-title',
        ),
        'path' => array(
          'data'  => l($endpoint->path, $endpoint->path),
          'class' => 'services-endpoint-path',
        ),
        'server' => array(
          'data'  => check_plain($endpoint->server),
          'class' => 'services-endpoint-server',
        ),
        'authentication' => array(
          'data'  => check_plain(join($auth_names, ', ')),
          'class' => 'services-endpoint-authentication',
        ),
        'storage' => array(
          'data'  => ($endpoint->export_type == EXPORT_IN_CODE) ? t('In code') : t('In database'),
          'class' => 'services-endpoint-storage',
        ),
        'operations' => array(
          'data'  => theme('links', $operations),
          'class' => 'services-endpoint-operations',
        ),
      ),
      'class' => 'services-endpoint-' . $endpoint->name . (!empty($endpoint->disabled) ? ' services-endpoint-disabled' : ''),
    );
  }

  $table = theme('table', $header, $rows, array('id' => 'services-list-endpoint'));

  drupal_add_css(drupal_get_path('module', 'services') . '/css/services.admin.css');

  return $table;
}

/**
 * Handle the add endpoint page..
 */
function services_add_endpoint() {
  $endpoint = services_endpoint_new();
  drupal_set_title(t('Add endpoint'));
  return services_edit_endpoint($endpoint);
}

/**
 * Edit an endpoint.
 *
 * Called from both the add and edit points to provide for common flow.
 */
function services_edit_endpoint($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  if ($endpoint && !empty($endpoint->title)) {
    drupal_set_title(check_plain($endpoint->title));
  }
  return drupal_get_form('services_edit_form_endpoint', $endpoint);
}

/**
 * Form to edit the settings of an endpoint.
 */
function services_edit_form_endpoint(&$form_state, $endpoint) {
  // Loading runtime include as needed by services_auth_info().
  module_load_include('runtime.inc', 'services');

  $form = array();

  $form['eid'] = array(
    '#type'  => 'value',
    '#value' => isset($endpoint->eid) ? $endpoint->eid : '',
  );

  $form['endpoint_object'] = array(
    '#type'  => 'value',
    '#value' => $endpoint,
  );

  $form['name'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => $endpoint->name,
    '#title'         => t('Endpoint name'),
    '#description'   => t('A unique name used to identify this preset internally. It must be only be alpha characters and underscores. No spaces, numbers or uppercase characters.'),
    '#required'      => TRUE,
  );

  $form['title'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => $endpoint->title,
    '#title'         => t('Endpoint title'),
    '#required'      => TRUE,
  );

  $servers = services_get_servers();
  $server_opts = array(
    '' => t('-- Select a server'),
  );
  foreach ($servers as $server => $info) {
    $server_opts[$server] = $info['#name'];
  }
  $form['server'] = array(
    '#type'          => 'select',
    '#options'       => $server_opts,
    '#default_value' => $endpoint->server,
    '#title'         => t('Server'),
    '#description'   => t('Select a the server that should be used to handle requests to this endpoint.'),
    '#required'      => TRUE,
  );

  $form['path'] = array(
    '#type'          => 'textfield',
    '#size'          => 24,
    '#maxlength'     => 255,
    '#default_value' => $endpoint->path,
    '#title'         => t('Path to endpoint'),
    '#required'      => TRUE,
  );

  $auth_modules = module_implements('services_authentication_info');

  if (!empty($auth_modules)) {
    $auth_options = array();
    foreach ($auth_modules as $module) {
      $info = services_authentication_info($module);
      $auth_options[$module] = $info['#title'];
    }

    $form['authentication'] = array(
      '#type'          => 'checkboxes',
      '#options'       => $auth_options,
      '#default_value' => array_keys($endpoint->authentication),
      '#title'         => t('Authentication'),
      '#description'   => t('Choose which authentication schemes that should ' .
        'be used with your endpoint. If no authentication method is selected ' .
        'the standard Drupal session security is used.'),
    );
  }
  else {
    $form['authentication'] = array(
      '#type'          => 'item',
      '#title'         => t('Authentication'),
      '#description'   => t('No authentication modules are installed, standard ' .
        'Drupal session based security will be used.'),
    );
  }

  $label = (empty($endpoint->eid) && $endpoint->export_type != EXPORT_IN_CODE) ? t('Save and proceed') : t('Save');
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => $label,
  );

  return $form;
}

/**
 * Validate submission of the preset edit form.
 */
function services_edit_form_endpoint_validate($form, &$form_state) {
  // Test uniqueness of name:
  if (preg_match("/[^A-Za-z0-9_]/", $form_state['values']['endpoint'])) {
    form_error($form['name'], t('Endpoint name must be alphanumeric or underscores only.'));
  }
  else {
    $query = "SELECT eid FROM {services_endpoint} WHERE name = '%s'";
    $args  = array($form_state['values']['name']);
    if (!empty($form_state['values']['eid']) && is_numeric($form_state['values']['eid'])) {
      $query .= ' AND eid != %d';
      $args[] = $form_state['values']['eid'];
    }
    if (db_result(db_query($query, $args))) {
      form_error($form['name'], t('Endpoint name must be unique.'));
    }
  }
  //TODO: More validation? Eg. validate path? Transliteration etc?
}

/**
 * Process submission of the mini panel edit form.
 */
function services_edit_form_endpoint_submit($form, &$form_state) {
  $endpoint = $form_state['values']['endpoint_object'];

  $endpoint->name           = $form_state['values']['name'];
  $endpoint->title          = $form_state['values']['title'];
  $endpoint->server         = $form_state['values']['server'];
  $endpoint->path           = $form_state['values']['path'];

  // Set the authentication modules, and preserve the settings for modules
  // that already exist.
  $auth = array();
  foreach (array_keys($form_state['values']['authentication']) as $module) {
    if (isset($endpoint->authentication[$module])) {
      $auth[$module] = $endpoint->authentication[$module];
    }
    else {
      $auth[$module] = array();
    }
  }
  $endpoint->authentication = $auth;

  if (empty($endpoint->eid)) {
    drupal_set_message(t('Your new endpoint %title has been saved.', array('%title' => $endpoint->title)));
    services_endpoint_save($endpoint);
    $form_state['values']['eid'] = $endpoint->eid;
    $form_state['redirect'] = 'admin/build/services/' . $endpoint->name . '/authentication';
  }
  else {
    drupal_set_message(t('Your changes have been saved.'));
    services_endpoint_save($endpoint);
    $form_state['redirect'] = 'admin/build/services';
  }
}

/**
 * Page callback to export an endpoint to PHP code.
 */
function services_export_endpoint(&$form_state, $endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  drupal_set_title(check_plain($endpoint->title));
  $code = services_endpoint_export($endpoint);

  $lines = substr_count($code, "\n");
  $form['code'] = array(
    '#type'          => 'textarea',
    '#title'         => $endpoint->title,
    '#default_value' => $code,
    '#rows'          => $lines,
  );

  return $form;
}

/**
 * Provide a form to confirm deletion of an endpoint.
 */
function services_delete_confirm_endpoint(&$form_state, $endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  if ($endpoint->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
    $title  = t('Are you sure you want to revert the endpoint "@title"?', array('@title' => $endpoint->title));
    $submit = t('Revert');
  }
  elseif ($endpoint->export_type != EXPORT_IN_CODE) {
    $title  = t('Are you sure you want to delete the endpoint "@title"?', array('@title' => $endpoint->title));
    $submit = t('Delete');
  }
  else {
    drupal_not_found();
    die;
  }
  $form['endpoint'] = array('#type' => 'value', '#value' => $endpoint->name);
  $form['eid']      = array('#type' => 'value', '#value' => $endpoint->eid);
  return confirm_form($form,
    $title,
    !empty($_GET['destination']) ? $_GET['destination'] : 'admin/build/services',
    t('This action cannot be undone.'),
    $submit, t('Cancel')
  );
}

/**
 * Handle the submit button to delete an endpoint.
 */
function services_delete_confirm_endpoint_submit($form, &$form_state) {
  $endpoint = services_endpoint_load($form_state['values']['endpoint']);
  if ($endpoint->eid == $form_state['values']['eid']) {
    services_endpoint_delete($endpoint);
    $form_state['redirect'] = 'admin/build/services';
  }
}

/**
 * Enable a default endpoint.
 */
function services_enable_endpoint($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  ctools_include('export');
  ctools_export_set_status('services_endpoint', $endpoint->name, FALSE);
  drupal_goto();
}

/**
 * Disable a default endpoint.
 */
function services_disable_endpoint($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  ctools_include('export');
  ctools_export_set_status('services_endpoint', $endpoint->name, TRUE);
  drupal_goto();
}

/**
 * Configure authentication for a endpoint.
 */
function services_edit_endpoint_authentication($endpoint) {
  if (!is_object($endpoint)) {
    $endpoint = services_endpoint_load($endpoint);
  }
  if ($endpoint && !empty($endpoint->title)) {
    drupal_set_title(check_plain($endpoint->title));
  }
  return drupal_get_form('services_edit_form_endpoint_authentication', $endpoint);
}

/**
 * Endpoint authentication configuration form.
 */
function services_edit_form_endpoint_authentication(&$form_state, $endpoint) {
  // Loading runtime include as needed by services_authentication_info().
  module_load_include('runtime.inc', 'services');

  $form = array();
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
  else if (empty($endpoint->authentication)) {
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
          '#title' => $info['#title'],
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
 * Add resources to an endpoint.
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
 * Form to add resources to an endpoint.
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
        'class' => 'resource',
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
          '#collapsed'   => TRUE,
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
            '#collapsed'   => TRUE,
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

function services_edit_form_endpoint_resources_submit($form, $form_state) {
  $resources = $form_state['values']['resources'];
  $endpoint  = $form_state['values']['endpoint_object'];

  foreach ($resources as $name => $resource) {
    $used = FALSE;
    $c = isset($endpoint->resources[$name]) ? $endpoint->resources[$name] : array();

    $c['alias'] = $resource['alias'];
    if (isset($resource['operations'])) {
      foreach ($resource['operations'] as $op => $def) {
        $cop = isset($c['operations'][$op]) ? $c['operations'][$op] : array();
        $cop = array_merge($cop, $def);
        if ($cop['enabled']) {
          $c['operations'][$op] = $cop;
          $used = $used || TRUE;
        }
        else {
          unset($c['operations'][$op]);
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
          $cop = isset($c[$class][$act]) ? $c[$class][$act] : array();
          $cop = array_merge($cop, $def);
          if ($cop['enabled']) {
            $c[$class][$act] = $cop;
            $class_used = $class_used || TRUE;
          }
          else {
            unset($c[$class][$act]);
          }
        }
        if (!$class_used) {
          unset($c[$class]);
        }
        $used = $class_used || $used;
      }
    }

    if ($used) {
      $endpoint->resources[$name] = $c;
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
function get_resource_operation_info($resource, $class, $name = NULL) {
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

  if ($rop = get_resource_operation_info($resource, $class, $name)) {
    $settings['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enabled'),
      '#default_value' => !empty($rop['endpoint']) && $rop['endpoint']['enabled'],
    );

    if ($rop['endpoint']['preprocess']) {
      $settings['preprocess'] = array(
        '#type' => 'item',
        '#title' => t('Preprocess function'),
        '#value' => $rop['endpoint']['preprocess'],
      );
    }

    if ($rop['endpoint']['postprocess']) {
      $settings['preprocess'] = array(
        '#type' => 'item',
        '#title' => t('Postprocess function'),
        '#value' => $rop['endpoint']['Postprocess'],
      );
    }

    // Let authentication modules add their configuration options
    foreach ($endpoint->authentication as $auth_module => $auth_settings) {
      $settings_form = services_auth_invoke($auth_module, 'controller_settings', $auth_settings, $rop, $endpoint, $class, $name);
      if (!empty($settings_form)) {
        $settings[$auth_module] = $settings_form;
      }
    }

    drupal_alter('services_resource_operation_settings', $settings, $endpoint, $resource, $class, $name);
  }
}
