self.add
  // R"(...)" is a raw string in which no escapes are needed - handy for regular expressions, which tend to include '\' a lot
  ( R"(\s+)", token_id::whitespace ) // \s = space, tab, newline et al.
  ( R"(\/\/[^\n]*|\/\*([^\*]|\*[^\/])*\*\/)", token_id::comment )

  // these keywords must come before identifier or they'd be matched by that
  ( R"(let)", token_id::let_ )
  ( R"(mutable)", token_id::mutable_ )
  // ...
  ( R"(\w+)", token_id::identifier ) // \w = alphanumeric and underscore

  ( R"(:)", token_id::colon )
  ( R"(;)", token_id::semicolon )
  // ...
  ;