-- Blossom's deliberately small C/C++ Neovim setup.  It is loaded only by
-- the embedded client, leaving a developer's normal Neovim configuration alone.
vim.o.termguicolors = true
vim.o.number = true
vim.o.relativenumber = false
vim.o.expandtab = true
vim.o.shiftwidth = 2
vim.o.tabstop = 2
vim.o.completeopt = "menuone,noselect"
vim.cmd("syntax enable")

vim.api.nvim_create_autocmd("FileType", {
  pattern = { "c", "cpp", "h", "hpp" },
  callback = function()
    if vim.fn.executable("clangd") == 1 then
      vim.lsp.start({ name = "clangd", cmd = { "clangd" } })
      vim.bo.omnifunc = "v:lua.vim.lsp.omnifunc"
    end
  end,
})

-- Built-in LSP completion stays intentionally lightweight.
vim.keymap.set("i", "<C-Space>", "<C-x><C-o>", { silent = true })

-- Blossom treats these as editor-window commands: save/close and discard/close.
vim.cmd([[cnoreabbrev <expr> qw getcmdtype() == ':' && getcmdline() ==# 'qw' ? 'wq' : 'qw']])
vim.cmd([[cnoreabbrev <expr> q getcmdtype() == ':' && getcmdline() ==# 'q' ? 'q!' : 'q']])
