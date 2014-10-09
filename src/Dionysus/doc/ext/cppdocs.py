from docutils               import nodes, utils
from docutils.parsers.rst   import directives
from sphinx                 import addnodes
from sphinx.util.compat     import Directive

from sphinx.directives.desc import CDesc

############
# CppClass #
############
class CppClass(nodes.Part, nodes.Element):
    @staticmethod
    def html_visit(visitor, node):
        visitor.body.append('<dl class="class">')
        visitor.body.append(visitor.starttag(node, 'dt'))
        if node['tparams']:
            visitor.body.append('<em class="property">template</em> &lt;')
            tparam_names = ['class ' + tp[0] for tp in node['tparams']]
            visitor.body.append(', '.join(tparam_names))
            visitor.body.append('&gt;<br/>')
            
        visitor.body.append('<em class="property">class</em> ')
        visitor.body.append('<tt class="descname">' + node['name'] + '</tt>')
        visitor.body.append(u'<a class="headerlink" href="#%s" title="%s">\u00B6</a>' % 
                              ('cppclass-'+node['name'], _('Permalink to this class')))
        visitor.body.append('</dt>')
        visitor.body.append('<dd>')

        if node['derives']:
            visitor.body.append('<p>Derives from ')
            for d in node['derives'].split(','):
                dnode ={ 'type': 'cppclass', 'target': d.strip() } 
                Ref.html_visit(visitor, dnode)
                visitor.body.append(d)
                Ref.html_depart(visitor, dnode)
            visitor.body.append('.</p>')

        visitor.body.append('<p>')
        for name, desc in node['tparams']:
            visitor.body.append(name + ' &mdash; ' + desc + '<br/>')
        visitor.body.append('</p>')


    @staticmethod
    def html_depart(visitor, node):
        visitor.body.append('</dd></dl>')


class CppClassDirective(Directive):
    has_content = True
    required_arguments = 1
    final_argument_whitespace = True
    option_spec = { 'derives': directives.unchanged,
                    'tparam':  directives.unchanged }

    def run(self):
        env = self.state.document.settings.env

        cppclass = CppClass()
        cppclass['name'] = self.arguments[0]
        cppclass['derives'] = self.options.get('derives')

        targetname = '%s-%s' % ('cppclass', cppclass['name'])
        targetnode = nodes.target('', '', ids=[targetname])
        self.state.document.note_explicit_target(targetnode)

        indextext = _('%s (C++ class)') % cppclass['name']
        inode = addnodes.index(entries = [('single', indextext, 
                                           'cppclass-' + cppclass['name'], 
                                           cppclass['name'])])

        self.state.nested_parse(self.content, self.content_offset, cppclass)

        return [inode, targetnode, cppclass]

class TParam(nodes.Element):
    pass

class TParamDirective(Directive):
    required_arguments = 1
    optional_arguments = 1
    final_argument_whitespace = True

    def run(self):
        tparam = TParam()

        tparam['name'] = self.arguments[0]
        if len(self.arguments) > 1:
            tparam['description'] = self.arguments[1]

        return [tparam]


#############
# CppMethod #
#############
class CppMethod(nodes.Part, nodes.Element):
    @staticmethod
    def html_visit(visitor, node):
        visitor.body.append(visitor.starttag(node, 'dt'))
        visitor.body.append(node['name'])
        visitor.body.append(u'<a class="headerlink" href="#%s" title="%s">\u00B6</a>' % 
                             ('cppmethod-' + node['classname'] + '::' + node['name'], _('Permalink to this class')))
        visitor.body.append('</dt>')
        visitor.body.append('<dd>')

    @staticmethod
    def html_depart(visitor, node):
        visitor.body.append('</dd></dl>')


class CppMethodDirective(Directive):
    has_content = True
    required_arguments = 1
    final_argument_whitespace = True

    def run(self):
        env = self.state.document.settings.env

        cppmethod = CppMethod()
        cppmethod['name'] = self.arguments[0]            # TODO: parse name

        targetname = '%s-%s' % ('cppmethod', cppmethod['name'])
        targetnode = nodes.target('', '', ids=[targetname])
        self.state.document.note_explicit_target(targetnode)

        indextext = _('%s (C++ method)') % cppmethod['name']
        inode = addnodes.index(entries = [('single', indextext, 
                                           'cppmethod-' + cppmethod['name'], 
                                           cppmethod['name'])])

        self.state.nested_parse(self.content, self.content_offset, cppmethod)

        return [inode, targetnode, cppmethod]


class Ref(nodes.Inline, nodes.TextElement):
    @staticmethod
    def html_visit(visitor, node):
        if node['type'] == 'cppclass':
            visitor.body.append('<a href="#%s-%s">' % (node['type'], node['target']))
        elif node['type'] == 'cppmethod':
            # TODO: check if the name is not fully qualified, and has a parent CppClass node, 
            #       in that case, prepend the name, otherwise
            visitor.body.append('<a href="#%s-%s::%s">' % (node['type'], node['classname'], node['target']))


    @staticmethod
    def html_depart(visitor, node):
        visitor.body.append('</a>')

def cppclass_role(role, rawtext ,text, lineno, inliner, options={}, content=[]):
    text = utils.unescape(text)
    node = Ref(text, text, target=text, type='cppclass')
    node['docname'] = inliner.document.settings.env.docname
    return [node], []

def cppmethod_role(role, rawtext ,text, lineno, inliner, options={}, content=[]):
    text = utils.unescape(text)
    node = Ref(text, text, target=text, type='cppmethod')
    node['docname'] = inliner.document.settings.env.docname
    return [node], []


def process_classnames(app, doctree, fromdocname):
    for node in doctree.traverse(CppClass):
        for method in node.traverse(CppMethod):
            method['classname'] = node['name']
        for ref in node.traverse(Ref):
            if ref['type'] == 'cppmethod':
                ref['classname'] = node['name']

def process_tparams(app, doctree, fromdocname):
    for node in doctree.traverse(CppClass):
        node['tparams'] = []
        for tparam in node.traverse(TParam):
            node['tparams'].append((tparam['name'], tparam['description']))


def process_cfunction_scope(app, doctree, fromdocname):
    for node in doctree.traverse():
        if 'ctype' in node: print node
        if 'cfunction' in node: print node
        
def setup(app):
    app.add_node(Ref,               html=(Ref.html_visit, Ref.html_depart))

    app.add_directive('cppclass',   CppClassDirective)
    app.add_node(CppClass,          html=(CppClass.html_visit, CppClass.html_depart))
    app.add_role('cppclass',        cppclass_role)
    
    app.add_directive('tparam',     TParamDirective)
    app.add_node(TParam,            html=(lambda v,n: '', lambda v,n: ''))
    
    app.add_directive('cppmethod',  CppMethodDirective)
    app.add_node(CppMethod,         html=(CppMethod.html_visit, CppMethod.html_depart))
    app.add_role('cppmethod',       cppmethod_role)

    app.connect('doctree-resolved', process_classnames)
    app.connect('doctree-resolved', process_tparams)
    
    app.connect('doctree-resolved', process_cfunction_scope)
