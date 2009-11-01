// $Id$

Drupal.file_embedBookmarks = {};

Drupal.behaviors.file_embed = function(context) {
  var translate = Drupal.settings.file_embed;
  // Body field text links.
  $('a.file-embed-select').click(function() {
    /*
     * Insetion key to calculate a current position index.
     */
    function insertionKey() {
      return '[file-' + (new Date()).valueOf() + ']';
    }

    var textarea_id = this.href.replace(/^.*[?|&]textarea=([^&]+).*$/, '$1');
    if (typeof tinyMCE != 'undefined' && tinyMCE.get(textarea_id) != null) {
      var editor = tinyMCE.get(textarea_id);
      if (editor.selection.getContent().length > 0) {
        Drupal.file_embedBookmarks[textarea_id] = editor.selection.getBookmark();
      }
      else if (tinymce.isIE && editor.getContent().length > 0) {
        // This is a big ugly hack for IE because for some reason
        // the thickbox iframe throws off the bookmark we'd normally
        // be using above.  Instead we brute force save our position
        // in the editor and use it during insert to slice our content
        // together.

        // First we store of the current text area id, for use in the thickbox
        // close method (just in case we need it).
        Drupal.file_embedCurrentTextArea = textarea_id;
        // Create a key to insert into the content.
        var insertionKey = insertionKey();
        // Insert the key.  This insert works.  For some reason when
        // you start mixing in the thickbox and its iframe, the mceInsertContent
        // command loses position in IE.  Go figure.
        editor.execCommand('mceInsertContent', false, insertionKey);
        // Now get the contents with the key embedded.
        var html = editor.getContent();
        // Use the position of the key as our insertion index
        // and store it for future reference
        Drupal.file_embedInsertIndex = html.indexOf(insertionKey);
        // Now remove the key so we don't scare anyone (if we
        // haven't done so already).
        editor.setContent(html.replace(insertionKey, ''));
      }
      Drupal.file_embedPopup(this.href, translate.popupTitle);
    }
    else if (typeof FCKeditor != 'undefined' && fckLaunchedTextareaId.indexOf(textarea_id) != -1 && $('#fck_' + fckLaunchedJsId[fckLaunchedTextareaId.indexOf(textarea_id)]).css("display") != "none") {
      // The last condition is needed as no surprise for the IE.
      // Similar to tinyMCE IE case.
      var editorId = fckLaunchedJsId[fckLaunchedTextareaId.indexOf(textarea_id)];
      var editor = FCKeditorAPI.GetInstance(editorId);
      if (editor.EditMode != FCK_EDITMODE_WYSIWYG) {
        return false;
      }
      var insertionKey = insertionKey();
      editor.InsertHtml(insertionKey);
      var html = editor.GetHTML();
      Drupal.file_embedInsertIndex = html.indexOf(insertionKey);
      editor.SetHTML(html.replace(insertionKey, ''));
      Drupal.file_embedPopup(this.href, translate.popupTitle);
    }
    else {
      var textarea = $('textarea#' + this.href.replace(/^.*[?|&]textarea=([^&]+).*$/, '$1'));
      var range = $(textarea).getSelection();
      Drupal.file_embedPopup(this.href.replace(/(^.*)(TB_iframe.*$)/, '$1textarea_start=' + range.start + '&textarea_end=' + range.end + '&$2'), translate.popupTitle);
    }
    return false;
  });
};

Drupal.file_embedPopup = function(url, title) {
  // Thickbox.js is pretty buggy; removing all its DIV elements before
  // attempting to show a thickbox overlay will ensure that the overlay
  // doesn't duplicate after the user has cancelled an earlier overlay.
  $("#TB_load").remove();
  $("#TB_overlay").remove();
  $("#TB_window").remove();
  tb_show(title || 'Add files', url, false);
};

Drupal.file_embedInsert = function(options) {
  /*
   * Insetrts string at given index.
   */
  function insertHTML(html, content) {
    // If index puts us inside a closing html tag, append a space and
    // move to the inside of the tag.
    var addSpace = false;
    if (html.substr(Drupal.file_embedInsertIndex-1, 1) == '<' && html.substr(Drupal.file_embedInsertIndex).match(/^\/\w+\>/)) {
      Drupal.file_embedInsertIndex -= 1;
      addSpace = true;
    }

    // Build up our new string.
    return html.slice(0, Drupal.file_embedInsertIndex) + (addSpace ? ' ' : '') + content + html.slice(html.slice(0, Drupal.file_embedInsertIndex).length);
  }

  var textarea_id = options['textarea'];
  var nid = options['nid'];
  var content = '[file:' + nid + (options['handler'] ? ' handler=' + options['handler'] : '') + (options['align'] ? ' align=' + options['align'] : '') +
    //(Number(options['padding']) > 0 ? ' padding=' + options['padding'] : '') +
    (options['link'] ? ' link=1' : '') + (options['caption'] ? ' caption=' + options['caption'] : '') + ']';

  // Special case for the TinyMCE WYSIWYG editor provided by tinymce.module.
  if (typeof tinyMCE != 'undefined' && tinyMCE.get(textarea_id) != null) {
  // TinyMCE can be tricky; we'll attempt inserting the snippet into the
  // WYSIWYG editor, but will also silently prepare to eat up any
  // curveball TinyMCE may throw at us. In case of trouble, we'll simply
  // fall back to attempting to use the standard textarea. One of the two
  // methods better work.
    try {
      var editor = tinyMCE.get(textarea_id);
      // If we got luck, we'd have a bookmark to use as our placement.
      if (Drupal.file_embedBookmarks[textarea_id] != null) {
        editor.selection.moveToBookmark(Drupal.file_embedBookmarks[textarea_id]);
        editor.selection.setContent(content);
        Drupal.file_embedBookmarks[textarea_id] = null;
      } 
      // Otherwise, IE is acting normal and not following the rules
      // so we have to beat it into submission.
      else if (tinymce.isIE && Drupal.file_embedInsertIndex > 0) {
        editor.setContent(insertHTML(editor.getContent(), content));
        Drupal.file_embedInsertIndex = null;
      }
      // And all other browsers... yes, that's 1 line there.
      else {
        editor.execCommand('mceInsertContent', false, content);
      }
    } catch(e) {}
  }
  else if (typeof FCKeditor != 'undefined' && fckLaunchedTextareaId.indexOf(textarea_id) != -1 && $('#fck_' + fckLaunchedJsId[fckLaunchedTextareaId.indexOf(textarea_id)]).css("display") != "none") {
    // The last condition is needed as no surprise for the IE.
    // Similar to tinyMCE IE case.
    try {
      var editorId = fckLaunchedJsId[fckLaunchedTextareaId.indexOf(textarea_id)];
      var editor = FCKeditorAPI.GetInstance(editorId);
      editor.SetHTML(insertHTML(editor.GetHTML(), content));
      Drupal.file_embedInsertIndex = null;
    } catch(e) {}
  }
  else {
    var textarea = $('textarea#' + textarea_id);
    var text = textarea.val();
    textarea.val(text.substring(0, options['textarea_start']) + content + text.substring(options['textarea_end'], text.length));
  }
};

/*
 * It occurs that indexOf is not defined in IE.
 */
if(Array.indexOf = 'undefined' || !Array.indexOf) {
  Array.prototype.indexOf = function(obj) {
    for(var i=0; i<this.length; i++) {
      if(this[i] == obj) {
        return i;
      }
    }
    return -1;
  }
}

