// $Id$

/**
 * Setting initial variables.
 */
Drupal.file_browserVars = {
  'selectedID':null, // selected object id
  'pageX':0, // where the mouseX pointer is
  'pageY':0, // where the mouseY pointer is
  'dropDownTimeoutId':0, // timeout id for hiding action div
  'filecount':1, // current number of file inputs
  'messageTimeout':5000, // timeout to show drupal mesages
  'progressTimeoutId':0, // id of the progress bar timeout
  'animationTimeout':2000 // fadeIn, fadeOut time
};

/**
 * Behaviours are bound to the Drupal namespace.
 */
Drupal.behaviors.file_browser = function(context) {  
  $('.file-dropdown').mouseout(function() {
    Drupal.file_browserVars.dropDownTimeoutId = setTimeout(function() { $('.file-dropdown').hide(); }, 1000); // hide action div
  }).mouseover(function() {
    clearTimeout(Drupal.file_browserVars.dropDownTimeoutId); // remove timeout for hiding the action div
  });

  $(document).mousemove(function(e) {
    // msie does not support pageX so handle that incase needed
    Drupal.file_browserVars.pageX = e.pageX ? e.pageX : e.clientX;
    Drupal.file_browserVars.pageY = e.pageY ? e.pageY : e.clientY;
    if ($.browser['msie'] == true && $.browser.version < 7.0) { Drupal.file_browserVars.pageY += 10; };
  });

  $('#edit-upload-0').change(function() {
    Drupal.file_browserCheckAddFileWidget();
  });
};
 
/**
 * Set the height of the file system holder based off the window height.
 */
Drupal.file_browserInit = function(block) {
  var ratio = block == 'page' ? 0.75 : 0.3;
  $('div.file-system').height('' + (Drupal.file_browserGetWindowHeight() * ratio) + 'px');
  var settings = Drupal.settings.file_browser;
  $('#file-preview').show().html(settings.no_file_selected);
};

/**
 * Gets window height.
 */
Drupal.file_browserGetWindowHeight = function() {
  if (typeof(window.innerHeight) == 'number')
    return window.innerHeight; // !MSIE
  if (document.documentElement && document.documentElement.clientHeight)
    return document.documentElement.clientHeight; // MSIE6
  if (document.body && document.body.clientHeight)
    return document.body.clientHeight; // MSIE4
  return 600; // reasonable default
};

/**
 * Set the vocabulary and term IDs on the folder.
 */
Drupal.file_browserSetContext = function(id, parentId) {
  var settings = Drupal.settings.file_browser;
  var folderId = id.match(/file-folder/) ? id : parentId;
  var isTerm = id.match(/-([vt])[\d]+/)[1] == 't' ? true : false;
  var tid = isTerm ? id.match(/-[vt]([\d]+)/)[1] : 0;

  // setting the term id that the file will be uploaded too
  $('#file-upload-tid').val(tid);
  // retrieve vid from the file system for use in the create term form
  var filesysvid = $('#' + folderId).parents().find('.file-system').attr('id');
  var vid = filesysvid.match(/file-system-([\d]+)/);
  // if vid not in the file system folder get it from the file folder
  if (!vid) {
    while (!id.match(/-v([\d]+)/))
      id = $('#' + id).parent().attr('id');
    vid = id.match(/-v([\d]+)/)[1];
  }
  // setting the vocabulary id that the term will be created under
  $('#newterm-vid').val(vid);
  // setting the term id the file will be created under
  $('#newterm-parent').val(tid);
  // toggle the file upload button
  var button = $('input#edit-upload-submit');
  isTerm ? button.removeAttr('disabled') : button.attr('disabled', 'disabled');
  // toggle the new term creation button
  button = $('input#edit-newterm-submit');
  $('#' + folderId).hasClass('hierarchy') ? button.removeAttr('disabled') : button.attr('disabled', 'disabled');
  // select and disable current group checkbox
  $('#file-browser-upload-form').find('.form-checkbox').each(function() { this.checked = false; }).removeAttr('disabled');
  if (folderId.match(/-g([\d]+)/)) {
    var gid = id.match(/-g([\d]+)/)[1];
    $('#edit-og-groups-' + gid).each(function() { this.checked = true; }).attr('disabled', 'disabled');
  }
  $('#file-preview-spinner').hide();
  $('#file-preview').show().html(settings.no_file_selected)
};

/**
 * Dynamically add the new Term to the DOM in the correct location
 * @param block {String} browser id
 * @param tid {Integer} term which we add
 * @param ptid {Integer} parent term to which we are appending the new node under
 * @param vid {Integer} if parent term is 0 then insert node under the correct vocabulary
 * @param gid {Integer} a og_vocab group id or 0
 * @param node {String} html representation of the node
 * @param msg {String} message to display on screen to the user
 * @param nodename {String} name of the new node being created
 */
Drupal.file_browserDisplayTerm = function(block, tid, ptid, vid, gid, node, msg, nodename) {
  var folder = 'file-folder-' + (ptid == 0 ? 'v' + vid : 't' + ptid) + '-g' + gid + '-b' + block;
  var term = 'file-folder-t' + tid + '-g' + gid + '-b' + block;
  var check = 0;
  var child = 0;
  $('#' + folder).children().each(function() {
    if (this.id.match(/file-folder-/)) {
      var name = $(this).find('.title').text();
      if (name.toLowerCase() > nodename.toLowerCase() && check == 0) {
        check = 1;
        $(node).insertBefore($(this));
      };
      child = 1;
    };
  });
  // the node might be the last one or there is no child nodes
  if (check == 0) {
    // if there were no child nodes
    if (child == 0)
      $('#' + folder + ' div.file-cells:first').after(node);
    else
      $('#' + folder + ' div.file-folder:last').after(node);
  };
  // remove empty class
  if ($('#' + folder).hasClass('empty')) {
    $('#' + folder).removeClass('empty')
  }
  // remove the term if the folder is not expanded
  if (!$('#' + folder).is('.expanded')) {
    $('#' + term).remove();
    // now expand the parent shelf
    $('#' + folder + ' div.file-cells:first').each(function() { Drupal.file_browserFolderClick(this, term) });
  }
  // display the term
  $('#' + term).fadeIn(Drupal.file_browserVars.animationTimeout);
  setTimeout('$(\'#' + term + '\').css(\'display\', \'block\')', Drupal.file_browserVars.animationTimeout);
  // select the newly created folder
  $('#' + term + ' div.file-cells:first').each(function() { Drupal.file_browserFolderClick(this) });
  Drupal.file_browserSetMsg(msg);
  $('#file-browser-newterm-form input#newterm-name').val(''); // resetting the value in the text field
};

/**
 * Dynamically add the new node to the DOM in the correct location
 * @param block {String} browser id
 * @param nid {Integer} term which we add
 * @param tid {Integer} parent term where the new node is being inserted into
 * @param node {String} html representation of the node
 * @param msg {String} message to display on screen to the user
 * @param nodename {String} name of the new node being created
 * @param last {Boolean} true if several nodes are added in bulk and this node is the last one
 */
Drupal.file_browserDisplayNode = function(block, nid, ptid, gid, node, msg, nodename, last) {
  var folder = 'file-folder-t' + ptid + '-g' + gid + '-b' + block;
  var file = 'file-node-t' + ptid + '-n' + nid + '-b' + block;
  var check = 0;
  $('#' + folder).children().each(function() {
    if (this.id.match(/file-node-/)) {
      var name = $(this).find('.title').text();
      if (name.toLowerCase() > nodename.toLowerCase() && check == 0) {
        check = 1;
        $(node).insertBefore($(this));
      };
    };
  });
  // the node might be the last one or there is no child nodes
  if (check == 0) {
      $('#' + folder).append(node);
  };
  // remove empty class
  if ($('#' + folder).hasClass('empty')) {
    $('#' + folder).removeClass('empty')
  }
  // hide the term if the folder is not expanded
  if (!$('#' + folder).is('.expanded')) {
    $('#' + file).remove();
    // now expand the parent shelf
    if (last) {
      $('#' + folder + ' div.file-cells:first').each(function() { Drupal.file_browserFolderClick(this, file) });
    }
  }
  else {
    // display the term
    $('#' + file).fadeIn(Drupal.file_browserVars.animationTimeout);
    setTimeout('$(\'#' + file + '\').css(\'display\', \'block\')', Drupal.file_browserVars.animationTimeout);
  }
  if (last) {
    // select the newly created file
    $('#' + file + ' div.file-cells:first').each(function() { Drupal.file_browserFileClick(this) });
    // resetting the form for file upload back to its original state
    if ($('#file_browser').find('input:file').size() > 1) {
      $('#file_browser').find('input:file').each(function() {
        if (this.id != 'edit-upload') {
          $(this).parent().remove(); // remove it from the DOM we do not need it, parent is the <p>
        }
      });
    }
  }
  Drupal.file_browserSetMsg(msg);
};

/**
 * Adds informational messages to the screen for the user to see
 * @param msg {String} message to be displayed on the screen to the user
 */
Drupal.file_browserSetMsg = function(msg) {
  if (!$('#file-upload-messages').size())
    $('<div id="file-upload-messages" class="messages status"></div>').insertBefore('.file-system');
  $('#file-upload-messages').append(msg);
  setTimeout(function() { $('#file-upload-messages').remove(); }, Drupal.file_browserVars.messageTimeout); // clear the messages after period of time
};

/**
 * Display any error messages on the screen since we do not do full page refreshes for actions
 * @param msg {String} error message to be displayed to the user
 */
Drupal.file_browserErrorMsg = function(msg) {
  if (!$('#file-upload-errors').size())
    $('<div id="file-upload-errors" class="messages error"></div>').insertBefore('.file-system');
  $('#file-upload-errors').append(msg);
  setTimeout(function() { $('#file-upload-errors').remove(); }, Drupal.file_browserVars.messageTimeout); // clear the messages after period of time
};

/**
 * Display any status messages on the screen since we do not do full page refreshes for actions
 * @param msg {String} status message to be displayed to the user
 */
Drupal.file_browserStatusMsg = function(msg) {
  if (!$('#file-upload-statuses').size())
    $('<div id="file-upload-statuses" class="messages status"></div>').insertBefore('.file-system');
  $('#file-upload-statuses').append(msg);
  setTimeout(function() { $('#file-upload-statuses').remove(); }, Drupal.file_browserVars.messageTimeout); // clear the messages after period of time
};

/**
 * Display any notice messages on the screen since we do not do full page refreshes for actions
 * @param msg {String} status message to be displayed to the user
 */
Drupal.file_browserNoticeMsg = function(msg) {
  if (!$('#file-upload-notices').size())
    $('<div id="file-upload-notices" class="messages notice"></div>').insertBefore('.file-system');
  $('#file-upload-notices').append(msg);
  setTimeout(function() { $('#file-upload-notices').remove(); }, Drupal.file_browserVars.messageTimeout); // clear the messages after period of time
};

/**
 * Highlights the selected row and adds necessary css classes to the pertinent rows
 * @param id {String} Parent id of the selected DOM object
 * @param folder {Boolean} Is it a folder {true = folder, false = not a folder}
 * @param toggle {Boolean} If true, 'expanded' class will be toggled
 */
Drupal.file_browserSelectRow = function(id, folder, toggle) {
  if (id != Drupal.file_browserVars.selectedId)
    if (Drupal.file_browserVars.selectedId && ($('#' + Drupal.file_browserVars.selectedId)))
      $('#' + Drupal.file_browserVars.selectedId).children('.file-cells').removeClass('selected');
  Drupal.file_browserVars.selectedId = id;
  $('#' + id).children('.file-cells').addClass('selected');

  if (folder && toggle)
    $('#' + id).toggleClass('expanded');
};

/*
 * Expands or collapses the folder that was selected based on the current DOM information
 * @param obj {Object} The folder object that was clicked on
 * @param elm {String} The id of the element to click on the folder expand
 */
Drupal.file_browserFolderClick = function(obj, elm) {
  var settings = Drupal.settings.file_browser;
  var parent = obj.parentNode;
  var id = parent.id;
  var block = id.match(/-b([^-]+)/)[1];
  var parentId = null;
  if (id.match(/-t([\d]+)/)) {
    tid = 'term/' + id.match(/-t([\d]+)/)[1];
    parentId = obj.parentNode.parentNode.id;
  }
  else {
    tid = 'voc/' + id.match(/-v([\d]+)/)[1];
  }
  // highlighting the row that was selected
  Drupal.file_browserSelectRow(id, true, true);
  // hiding the block
  $('#file-preview').hide();
  // upload and newterm are only on the main page
  if (block == 'page') {
    Drupal.file_browserSetContext(id, parentId);
  }
  // if the shelf currently is closed then open the shelf
  // the class 'expanded' is already toggled in SelectRow.
  if ($(parent).hasClass('expanded')) {
    if (!$(parent).hasClass('empty')) {
      $('#' + id + '-spinner').show();
    }
    $.get(settings.ajax_url + '/' + tid + '/' + block, function(result) {
      $(obj).parent().addClass('expanded');
      // to stop duplicate info from double clicks
      if (parent.childNodes.length == 1) {
        $('#' + id).append(result);
        $('#' + id).find('span.file.with-menu').click(function(event) {
          $(this).toggleClass('active');
          $(this).find('ul').toggle();
        });
        $('a.file-metadata').cluetip({arrows: true});
      }
      $('#' + id + '-spinner').hide();
      if (typeof(elm) != 'undefined') {
        // click the element
        if (elm.match(/file-folder/))
          $('#' + elm + ' div.file-cells:first').each(function() { Drupal.file_browserFolderClick(this) });
        else
          $('#' + elm + ' div.file-cells:first').each(function() { Drupal.file_browserFileClick(this) });
      }
    });
  } else {
    // shelf is open hence remove all the children except the shelf name
    $(obj.parentNode).children().each(function() {
      if (this.nodeName.toLowerCase() == 'div' && this.id)
        parent.removeChild(this);
      else if (this.nodeName.toLowerCase() == 'script')
        parent.removeChild(this);
    });
  }
  return false;
};

/**
 * Handle operations when one of the file rows is clicked
 * @param obj {Object} File Object that holds a file which was clicked
 */
Drupal.file_browserFileClick = function(obj) {
  var id = obj.parentNode.id;
  var nid = id.match(/-n([\d]+)/)[1]; // node id
  var block = id.match(/-b([^-]+)/)[1];
  var tid = id.match(/-t([\d]+)/)[1]; // term id
  var parentId = obj.parentNode.parentNode.id;
  // highlighting the row that was selected
  Drupal.file_browserSelectRow(id, false, true);
  // show preview
  if (block == 'page') {
    Drupal.file_browserSetContext(id, parentId);
    $('#file-preview').hide();
    $('#file-preview-spinner').show();
    $.get($('#file-preview-url').val() + '/' + nid + '/' + tid, function(result) {
      $('#file-preview-spinner').hide();
      $('#file-preview').html(result).show();
      if (typeof collapseAutoAttach != 'undefined') {
        collapseAutoAttach();
      };
    });
  }
  return false;
};

/**
 * Adds upload widget.
 */
Drupal.file_browserAddFileWidget = function() {
  // creating the new file widget that will be added to the form
  var widget = document.createElement("input");
  widget.setAttribute("type", "file");
  widget.setAttribute("name", "files[upload_" + Drupal.file_browserVars.filecount + "]");
  widget.setAttribute("id", "edit-upload-" + Drupal.file_browserVars.filecount);
  widget.setAttribute("class", "form-file");
  widget.setAttribute("size", "10");
  // creating widget wrapper
  var wrapper = document.createElement("div");
  wrapper.setAttribute('id', 'edit-upload-' + Drupal.file_browserVars.filecount + '-wrapper');
  wrapper.setAttribute('class', 'form-item');
  wrapper.appendChild(widget);
  var div = document.createElement("div");
  div.appendChild(wrapper);
  // adding the new widget to the container widget
  $('#file-upload').find('input:file').each(function() {
    if (this.id == 'edit-upload-' + (Drupal.file_browserVars.filecount - 1)) {
      $(div).insertAfter($(this).parent().parent());
    }
  });
  // binding on change function
  $('#edit-upload-' + Drupal.file_browserVars.filecount).change(function() {
    Drupal.file_browserCheckAddFileWidget();
  });
  // integration with file restriction module
  if (typeof(Drupal.file_restrictionExtensions) == 'function') {
    Drupal.file_restrictionExtensions('#edit-upload-' + Drupal.file_browserVars.filecount);
  }
  Drupal.file_browserVars.filecount++;
};

/**
 * Checks if add upload widget is needed.
 */
Drupal.file_browserCheckAddFileWidget = function() {
  var empty = false;
  for (var i=0;i<Drupal.file_browserVars.filecount;i++) {
    if ($('#edit-upload-' + i).val() == '') {
      empty = true;
    }
  }
  if (!empty) {
    Drupal.file_browserAddFileWidget();
  }
}

/**
 * Removes upload widgets.
 */
Drupal.file_browserDelFileWidget = function() {
  for (var i=1;i<Drupal.file_browserVars.filecount;i++) {
    $('#edit-upload-' + i).parent().remove();
  }
  Drupal.file_browserVars.filecount = 1;
  $('#file-browser-upload-form input#edit-upload-0').val(''); // resetting the value in the text field
  $('.file-restriction').hide();
  $('#file-upload-progress').hide();
  var settings = Drupal.settings.file_browser;
  if (settings.apc) {
    $('#edit-progress-key').val(settings.progress_id); // reset the APC id
    $('div.filled').css('width', '0%');
    $('div.percentage').html('');
    clearTimeout(Drupal.file_browserVars.progressTimeoutId);
  }
};

/**
 * Sets parent term values on the file upload.
 */
Drupal.file_browserUpdateTerm = function(id, size, files) {
  $('#' + id).children().each(function() {
    if (this.id == '') {
      $(this).find('.file-size').text(size);
      $(this).find('.file-date').text(files);
    }
  })
};

/**
 * Handle operations when file upload is clicked
 * @param obj {Object} File Upload Object <a> tag
 */
Drupal.file_browserFileUploadClick = function(obj) {
  setTimeout(function() { $('input#edit-upload-submit').attr('disabled', 'disabled'); }, 100);
  var settings = Drupal.settings.file_browser;
  $('#file-upload-progress').show();
  if (settings.apc) {
    Drupal.file_browserVars.progressTimeoutId = setTimeout('Drupal.file_browserGetProgress(' + settings.progress_timeout + ')', settings.progress_interval);
  }
  return true;
};

/**
 * Get the progress bar
 * @param timeout {Integer} If the APC is installed but not configured,
 *   the script will make timout times connections untill it times out.
 */
Drupal.file_browserGetProgress = function(timeout) {
  var settings = Drupal.settings.file_browser;
  $.getJSON(settings.progress_url, function(progress) {
    if (progress.percentage >= 0 && progress.percentage <= 100) {
      $('div.filled').css('width', progress.percentage +'%');
      $('div.percentage').html(progress.percentage +'%');
    }
    $('div.message').html(progress.message);
    if (progress.percentage < 100) {
      if (progress.percentage >= 0 || (progress.percentage == -1 && timeout > 1)) {
        Drupal.file_browserVars.progressTimeoutId = setTimeout('Drupal.file_browserGetProgress(' + (timeout-1) + ')', settings.progress_interval);
      }
    }
  });
};

