In order to be able to call Perseus functions in C++, these opcode snippets must be in the code. They handle the translation.

    execute_function( return_value_space, parameters, function_address ):
        indirect_call
        exit

    create_coroutine( function address )
        indirect_coroutine
        exit
    
    resume_coroutine( parameters, identifier )
        resume_pushing_everything
        exit
    
    coroutine_state( identifier ):
        coroutine_state
        exit
    
    delete_coroutine( identifier )
        coroutine_delete
        exit
