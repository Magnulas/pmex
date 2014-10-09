from docutils import nodes, utils
import posixpath

def sfile_role(typ, rawtext, etext, lineno, inliner, options={}, content=[]):
    env = inliner.document.settings.env
    baseuri = env.config.sfile_base_uri
    text = utils.unescape(etext)
    refnode = nodes.reference('', '', refuri=posixpath.join(baseuri, text))
    refnode += nodes.literal(text, text)
    return [refnode], []

def setup(app):
    app.add_role('sfile', sfile_role)
    app.add_config_value('sfile_base_uri', 'http://example.com/source', True) 
