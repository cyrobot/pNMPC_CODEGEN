/*
*    This file is part of pNMPC software.
*    Copyright (c) 2020 GIPSA lab [https://github.com/Kartz4code/pNMPC_CODEGEN]
*
*    Main developer - Karthik Murali Madhavan Rathai
*
*    pNMPC - A Code Generation Software Tool For Implementation of Derivative Free
*    Parameterized NMPC Scheme for Embedded Control Systems
*    The software was developed by Karthik Murali Madhavan Rathai under the supervision of
*    Mazen Alamir and Olivier Sename affiliated to Univ. Grenoble Alpes, CNRS, Grenoble INP,
*    GIPSA lab, 38000 Grenoble, France.
*
* 	 pNMPC software is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*	 pNMPC software is distributed in the hope that it will be useful,
*	 but WITHOUT ANY WARRANTY; without even the implied warranty of
*	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	 GNU General Public License for more details.
*
*	 You should have received a copy of the GNU General Public License
* 	 along with pNMPC software.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include "pNMPC_headers.hpp"
#include "ScalarConstraint.hpp"

size_t ScalarConstraint::_countconst{ 0 };
std::vector<std::unique_ptr<ScalarConstraint>> ScalarConstraint::_XConst(0);
size_t ScalarConstraint::_keepConstSize_reg{ 0 };
size_t ScalarConstraint::_keepConstSize_term{ 0 };

ScalarConstraint::ScalarConstraint() : EquationProcessor{ db->accessExpressionsConst(),
														  db->accessTokensConst(), 
														  db->accessOpSetConst(), 
													  	  _countconst }														 
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
	}
}


ScalarConstraint::ScalarConstraint(const HyperStates& S): EquationProcessor{ db->accessExpressionsConst(),
																			 db->accessTokensConst(),
																			 db->accessOpSetConst(),
																			 _countconst }
{ 
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_store = S; this->_h_store = this->_h_store*1;
		this->_h_MAT.resize(1, 1); this->_h_MAT[0][0] = this->_h_store;
		db->accessExpressionsConst().push_back(this->_h_store.getExpression());
		parse(); expression_tree();
		if (this->_ConstraintType == CONSTTYPE::REGULAR)
		{
			tree_reduction(_keepConstSize_reg);
			this->_keepConstSize_term = db->accessOpSetConst().size();
		}
		else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
		{
			tree_reduction(_keepConstSize_term);
			this->_keepConstSize_reg = db->accessOpSetConst().size();
		}
		db->accessScalarConstraintEquations().push_back(this); 
		++this->_countconst;
	}
}

ScalarConstraint::ScalarConstraint(const ScalarConstraint& SCS) : EquationProcessor{ db->accessExpressionsConst(),
																					 db->accessTokensConst(),
																					 db->accessOpSetConst(),
																					 _countconst }

{
	if (Builder::getConstBuild())
	{
		if (SCS.getMATHyperStates().getNumRows() == 0 && SCS.getMATHyperStates().getNumColumns() == 0)
		{
			if (Builder::getRegConstBuild())
				this->_ConstraintType = CONSTTYPE::REGULAR;
			if (Builder::getTermConstBuild())
				this->_ConstraintType = CONSTTYPE::TERMINAL;
			this->_h_store = SCS.getHyperStates(); this->_h_store = this->_h_store * 1;
			db->accessExpressionsConst().push_back(this->_h_store.getExpression());
			parse(); expression_tree();
			if (this->_ConstraintType == CONSTTYPE::REGULAR)
			{
				tree_reduction(_keepConstSize_reg);
				this->_keepConstSize_term = db->accessOpSetConst().size();
			}
			else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
			{
				tree_reduction(_keepConstSize_term);
				this->_keepConstSize_reg = db->accessOpSetConst().size();
			}
			db->accessScalarConstraintEquations().push_back(this);
			++this->_countconst;
		}
		else
		{
			this->_h_MAT = std::move(SCS.getMATHyperStates());
			if (this->_h_MAT.getNumColumns() == 1)
			{
				if (Builder::getRegConstBuild())
					this->_ConstraintType = CONSTTYPE::REGULAR;
				if (Builder::getTermConstBuild())
					this->_ConstraintType = CONSTTYPE::TERMINAL;
				for (size_t j{ 0 }; j < this->_h_MAT.getNumRows(); j++)
					this->_XConst.push_back(std::make_unique<ScalarConstraint>(this->_h_MAT[j][0]));
				if (this->_ConstraintType == CONSTTYPE::REGULAR)
				{
					tree_reduction(_keepConstSize_reg);
					this->_keepConstSize_term = db->accessOpSetConst().size();
				}
				else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
				{
					tree_reduction(_keepConstSize_term);
					this->_keepConstSize_reg = db->accessOpSetConst().size();
				}
			}
			else
				std::cerr << "Matrix forms not supported";
		}
	}
}


ScalarConstraint::ScalarConstraint(HyperStates&& S) : EquationProcessor{ db->accessExpressionsConst(),
																		 db->accessTokensConst(),	
																		 db->accessOpSetConst(),
																		 _countconst }
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_store = std::move(S); this->_h_store = this->_h_store*1;
		this->_h_MAT.resize(1, 1); this->_h_MAT[0][0] = this->_h_store;
		db->accessExpressionsConst().push_back(this->_h_store.getExpression());
		parse(); expression_tree();
		if (this->_ConstraintType == CONSTTYPE::REGULAR)
		{
			tree_reduction(_keepConstSize_reg);
			this->_keepConstSize_term = db->accessOpSetConst().size();
		}
		else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
		{
			tree_reduction(_keepConstSize_term);
			this->_keepConstSize_reg = db->accessOpSetConst().size();
		}
		db->accessScalarConstraintEquations().push_back(this);
		++this->_countconst;
	}
}

ScalarConstraint::ScalarConstraint(const MATHyperStates& HSC) : EquationProcessor{ db->accessExpressionsConst(),
																				   db->accessTokensConst(),
																				   db->accessOpSetConst(),
																				   _countconst }
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_MAT = std::move(HSC);
		if (HSC.getNumColumns() == 1)
		{
			for (size_t j{ 0 }; j < HSC.getNumRows(); j++)
				this->_XConst.push_back(std::make_unique<ScalarConstraint>(HSC.getMatrixElement(j,0)));
			if (this->_ConstraintType == CONSTTYPE::REGULAR)
			{
				tree_reduction(_keepConstSize_reg);
				this->_keepConstSize_term = db->accessOpSetConst().size();
			}
			else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
			{
				tree_reduction(_keepConstSize_term);
				this->_keepConstSize_reg = db->accessOpSetConst().size();
			}
		}
		else
			std::cerr << "Matrix forms not supported";
	}
}

ScalarConstraint::ScalarConstraint(MATHyperStates&& HSC) : EquationProcessor{ db->accessExpressionsConst(),
																			  db->accessTokensConst(),
																			  db->accessOpSetConst(),
																			  _countconst }
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_MAT = std::move(HSC);
		if (HSC.getNumColumns() == 1)
		{
			for (size_t j{ 0 }; j < HSC.getNumRows(); j++)
				this->_XConst.push_back(std::make_unique<ScalarConstraint>(HSC[j][0]));
			if (this->_ConstraintType == CONSTTYPE::REGULAR)
			{
				tree_reduction(_keepConstSize_reg);
				this->_keepConstSize_term = db->accessOpSetConst().size();
			}
			else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
			{
				tree_reduction(_keepConstSize_term);
				this->_keepConstSize_reg = db->accessOpSetConst().size();
			}
		}
		else
			std::cerr << "Matrix forms not supported";
	}
}

ScalarConstraint::ScalarConstraint(const Real&& val) : EquationProcessor{ db->accessExpressionsConst(),
																		  db->accessTokensConst(),
																		  db->accessOpSetConst(),
																		  _countconst }
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		db->accessExpressionsConst().push_back(std::to_string(val));
		parse(); expression_tree();
		if (this->_ConstraintType == CONSTTYPE::REGULAR)
		{
			tree_reduction(_keepConstSize_reg);
			this->_keepConstSize_term = db->accessOpSetConst().size();
		}
		else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
		{
			tree_reduction(_keepConstSize_term);
			this->_keepConstSize_reg = db->accessOpSetConst().size();
		}
		db->accessScalarConstraintEquations().push_back(this); 
		++this->_countconst;
	}
}

ScalarConstraint::ScalarConstraint(const EquationProcessor& EQ) : EquationProcessor{ db->accessExpressionsConst(),
																					 db->accessTokensConst(),
																					 db->accessOpSetConst(),
																					 _countconst }
{
	if (Builder::getConstBuild())
	{
		if (EQ.getMATHyperStates().getNumRows() == 0 && EQ.getMATHyperStates().getNumColumns() == 0)
		{
			if (Builder::getRegConstBuild())
				this->_ConstraintType = CONSTTYPE::REGULAR;
			if (Builder::getTermConstBuild())
				this->_ConstraintType = CONSTTYPE::TERMINAL;
			this->_h_store = EQ.getHyperStates(); this->_h_store = this->_h_store * 1;
			db->accessExpressionsConst().push_back(this->_h_store.getExpression());
			parse(); expression_tree();
			if (this->_ConstraintType == CONSTTYPE::REGULAR)
			{
				tree_reduction(_keepConstSize_reg);
				this->_keepConstSize_term = db->accessOpSetConst().size();
			}
			else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
			{
				tree_reduction(_keepConstSize_term);
				this->_keepConstSize_reg = db->accessOpSetConst().size();
			}
			db->accessScalarConstraintEquations().push_back(this);
			++this->_countconst;
		}
		else
		{
			this->_h_MAT = std::move(EQ.getMATHyperStates());
			if (this->_h_MAT.getNumColumns() == 1)
			{
				if (Builder::getRegConstBuild())
					this->_ConstraintType = CONSTTYPE::REGULAR;
				if (Builder::getTermConstBuild())
					this->_ConstraintType = CONSTTYPE::TERMINAL;
				for (size_t j{ 0 }; j < this->_h_MAT.getNumRows(); j++)
					this->_XConst.push_back(std::make_unique<ScalarConstraint>(this->_h_MAT[j][0]));
				if (this->_ConstraintType == CONSTTYPE::REGULAR)
				{
					tree_reduction(_keepConstSize_reg);
					this->_keepConstSize_term = db->accessOpSetConst().size();
				}
				else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
				{
					tree_reduction(_keepConstSize_term);
					this->_keepConstSize_reg = db->accessOpSetConst().size();
				}
			}
			else
				std::cerr << "Matrix forms not supported";
		}
	}
}

void ScalarConstraint::operator=(HyperStates&& S)
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_store = S; this->_h_store = this->_h_store*1;
		this->_h_MAT.resize(1, 1); this->_h_MAT[0][0] = this->_h_store;
		db->accessExpressionsConst().push_back(this->_h_store.getExpression());
		parse(); expression_tree();
		if (this->_ConstraintType == CONSTTYPE::REGULAR)
		{
			tree_reduction(_keepConstSize_reg);
			this->_keepConstSize_term = db->accessOpSetConst().size();
		}
		else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
		{
			tree_reduction(_keepConstSize_term);
			this->_keepConstSize_reg = db->accessOpSetConst().size();
		}
		db->accessScalarConstraintEquations().push_back(this);
		++this->_countconst;
	}
}

void ScalarConstraint::operator=(MATHyperStates&& HSC)
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		this->_h_MAT = std::move(HSC);
		if (HSC.getNumColumns() == 1)
		{
			for (size_t j{ 0 }; j < HSC.getNumRows(); j++)
				this->_XConst.push_back(std::make_unique<ScalarConstraint>(HSC[j][0]));
			if (this->_ConstraintType == CONSTTYPE::REGULAR)
			{
				tree_reduction(_keepConstSize_reg);
				this->_keepConstSize_term = db->accessOpSetConst().size();
			}
			else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
			{
				tree_reduction(_keepConstSize_term);
				this->_keepConstSize_reg = db->accessOpSetConst().size();
			}
		}
		else
			std::cerr << "Matrix forms not supported";
	}
}

void ScalarConstraint::operator=(Real&& val)
{
	if (Builder::getConstBuild())
	{
		if (Builder::getRegConstBuild())
			this->_ConstraintType = CONSTTYPE::REGULAR;
		if (Builder::getTermConstBuild())
			this->_ConstraintType = CONSTTYPE::TERMINAL;
		db->accessExpressionsConst().push_back(std::to_string(val));
		parse(); expression_tree();
		if (this->_ConstraintType == CONSTTYPE::REGULAR)
		{
			tree_reduction(_keepConstSize_reg);
			this->_keepConstSize_term = db->accessOpSetConst().size();
		}
		else if (this->_ConstraintType == CONSTTYPE::TERMINAL)
		{
			tree_reduction(_keepConstSize_term);
			this->_keepConstSize_reg = db->accessOpSetConst().size();
		}
		db->accessScalarConstraintEquations().push_back(this); 
		++this->_countconst;
	}
}

const CONSTTYPE& ScalarConstraint::getConstraintType() const
{
	return this->_ConstraintType;
}