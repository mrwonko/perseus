rule<> let_; // no result
rule<> mutable_;
//...

let_ = boost::spirit::qi::token( tokend_id::let_ );
mutable_ = boost::spirit::qi::token( tokend_id::mutable_ );