# Modelo de Pull Request

## 📋 Descrição
Descreva brevemente as mudanças implementadas neste PR.

## 🔄 Tipo de Mudança
Marque com `x` o tipo de mudança:

- [ ] 🐛 Bug fix (correção que resolve um problema)
- [ ] ✨ Nova funcionalidade (mudança que adiciona funcionalidade)
- [ ] 💥 Breaking change (mudança que quebra compatibilidade)
- [ ] 📚 Documentação (mudança apenas na documentação)
- [ ] 🎨 Style (formatação, sem mudança de lógica)
- [ ] ♻️ Refactoring (mudança que não corrige bugs nem adiciona funcionalidades)
- [ ] ⚡ Performance (mudança que melhora performance)
- [ ] ✅ Teste (adição ou correção de testes)
- [ ] 🔧 Build (mudanças no sistema de build ou dependências)

## 🧪 Como Testar
Descreva os passos para testar suas mudanças:

1. Compile o projeto: `.\build_mvc.bat` ou `make`
2. Execute: `bin\main.exe`
3. Teste a funcionalidade...

## 📸 Screenshots (se aplicável)
Adicione screenshots que demonstrem as mudanças visuais.

## ✅ Checklist
Marque com `x` os itens verificados:

### Código
- [ ] Meu código segue as convenções do projeto
- [ ] Realizei uma auto-revisão do código
- [ ] Comentei partes complexas do código
- [ ] Minhas mudanças não geram novos warnings
- [ ] Adicionei logs apropriados quando necessário

### Build e Testes
- [ ] Testei com MinGW/GCC build
- [ ] Testei com MSVC build (se disponível)
- [ ] Verifiquei que não há vazamentos de memória
- [ ] Testei em modo Debug e Release
- [ ] A aplicação inicia e funciona corretamente

### Documentação
- [ ] Atualizei o README.md se necessário
- [ ] Atualizei comentários no código
- [ ] Documentei funções públicas novas

### Arquitetura MVC
- [ ] Mudanças seguem a separação MVC
- [ ] Controllers contêm apenas lógica de negócio
- [ ] Forms contêm apenas interface
- [ ] Utils são reutilizáveis e sem estado

## 📝 Notas Adicionais
Adicione quaisquer notas, considerações ou decisões de design relevantes.

## 🔗 Issues Relacionadas
Closes #(número do issue)
Related to #(número do issue)
