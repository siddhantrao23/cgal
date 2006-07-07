// Copyright (c) 2005, 2006 Fernando Luis Cacciola Carballal. All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
#ifndef CGAL_POLYGON_OFFSET_BUILDER_TRAITS_2_H
#define CGAL_POLYGON_OFFSET_BUILDER_TRAITS_2_H 1

#include <CGAL/Straight_skeleton_2/Straight_skeleton_aux.h>
#include <CGAL/Straight_skeleton_2/Straight_skeleton_builder_traits_2_aux.h>
#include <CGAL/predicates/Polygon_offset_pred_ftC2.h>
#include <CGAL/constructions/Polygon_offset_cons_ftC2.h>

CGAL_BEGIN_NAMESPACE

namespace CGAL_SS_i {

template<class K>
struct Compare_offset_against_event_time_2 : Functor_base_2<K>
{
  typedef Functor_base_2<K> Base ;

  typedef typename Base::FT               FT ;
  typedef typename Base::Segment_2        Segment_2 ;
  typedef typename Base::Triedge_2        Triedge_2 ;
  typedef typename Base::Sorted_triedge_2 Sorted_triedge_2 ;

  typedef Uncertain<Comparison_result> result_type ;
  typedef Arity_tag<2>                 Arity ;

  Uncertain<Comparison_result> operator() ( FT aT, Sorted_triedge_2 const& aE ) const
  {
    return compare_offset_against_isec_timeC2(aT,aE) ;
  }
};


template<class K>
struct Construct_offset_point_2 : Functor_base_2<K>
{
  typedef Functor_base_2<K> Base ;

  typedef typename Base::FT        FT ;
  typedef typename Base::Point_2   Point_2 ;
  typedef typename Base::Segment_2 Segment_2 ;

  typedef boost::optional< Point_2 > result_type ;
  
  typedef Arity_tag<3> Arity ;

  result_type operator() ( FT const& aT, Segment_2 const& aE0, Segment_2 const& aE1 ) const
  {
    bool ok = false ;
    
    result_type p = construct_offset_pointC2(aT,aE0,aE1);
    if ( p )
      ok = is_point_calculation_accurate(aT,*p,aE0,aE1);
      
    return ok ? p : boost::none ;
  }
  
  bool is_point_calculation_accurate( double time, Point_2 const& p, Segment_2 const& aE0, Segment_2 const& aE1 ) const
  { 
    Point_2 const& e0s = aE0.source();
    Point_2 const& e0t = aE0.target();
    
    Point_2 const& e1s = aE1.source();
    Point_2 const& e1t = aE1.target();
  
    FT d0 = squared_distance_from_point_to_lineC2(p.x(),p.y(),e0s.x(),e0s.y(),e0t.x(),e0t.y());
    FT d1 = squared_distance_from_point_to_lineC2(p.x(),p.y(),e1s.x(),e1s.y(),e1t.x(),e1t.y());
  
    FT time2 = CGAL_NTS square(time);
    
    FT diff0 = CGAL_NTS square(d0-time2);
    FT diff1 = CGAL_NTS square(d1-time2);
    
    FT const eps = 1e-5 ;
    return diff0 < eps && diff1 < eps ;
  }

  template<class NT>  
  bool is_point_calculation_accurate( NT const& time, Point_2 const& p, Segment_2 const& aE0, Segment_2 const& aE1 ) const { return true ; }
};


} // namespace CGAL_SS_i

template<class K>
struct Polygon_offset_builder_traits_2_functors
{
  typedef CGAL_SS_i::Get_ss_triedge_collinearity_2      <K> Get_ss_triedge_collinearity_2 ;
  typedef CGAL_SS_i::Compare_offset_against_event_time_2<K> Compare_offset_against_event_time_2 ;
  typedef CGAL_SS_i::Construct_offset_point_2           <K> Construct_offset_point_2 ;
  typedef CGAL_SS_i::Construct_ss_triedge_2             <K> Construct_ss_triedge_2 ;
  typedef CGAL_SS_i::Construct_ss_sorted_triedge_2      <K> Construct_ss_sorted_triedge_2 ;
} ;

template<class K>
struct Polygon_offset_builder_traits_2_base
{
  typedef K Kernel ;
  
  typedef typename K::FT        FT ;
  typedef typename K::Point_2   Point_2 ;
  typedef typename K::Segment_2 Segment_2 ;
  
  typedef CGAL_SS_i::Triedge_2       <K> Triedge_2 ;
  typedef CGAL_SS_i::Sorted_triedge_2<K> Sorted_triedge_2 ;

  template<class F> F get( F const* = 0 ) const { return F(); }
} ;

template<class Is_filtered_kernel, class K> class Polygon_offset_builder_traits_2_impl ;

template<class K>
class Polygon_offset_builder_traits_2_impl<Tag_false,K> : public Polygon_offset_builder_traits_2_base<K>
{
  typedef Polygon_offset_builder_traits_2_functors<K> Unfiltering ;

public:

  typedef Unfiltered_predicate_adaptor<typename Unfiltering::Get_ss_triedge_collinearity_2>
    Get_ss_triedge_collinearity_2 ;
  
  typedef Unfiltered_predicate_adaptor<typename Unfiltering::Compare_offset_against_event_time_2>
    Compare_offset_against_event_time_2 ;

  typedef typename Unfiltering::Construct_offset_point_2      Construct_offset_point_2 ;
  typedef typename Unfiltering::Construct_ss_triedge_2        Construct_ss_triedge_2 ;
  typedef typename Unfiltering::Construct_ss_sorted_triedge_2 Construct_ss_sorted_triedge_2 ;

} ;

template<class K>
class Polygon_offset_builder_traits_2_impl<Tag_true,K> : public Polygon_offset_builder_traits_2_base<K>
{
  typedef typename K::EK EK ;
  typedef typename K::FK FK ;
  
  typedef Polygon_offset_builder_traits_2_functors<EK> Exact ;
  typedef Polygon_offset_builder_traits_2_functors<FK> Filtering ;
  typedef Polygon_offset_builder_traits_2_functors<K>  Unfiltering ;

  typedef Cartesian_converter< K, EK> BaseC2E;
  typedef Cartesian_converter< K, FK> BaseC2F;
  typedef Cartesian_converter< K,  K> BaseC2C;
  typedef Cartesian_converter<EK,  K> BaseE2C;
  
  typedef CGAL_SS_i::SS_converter<BaseC2E> C2E ;
  typedef CGAL_SS_i::SS_converter<BaseC2F> C2F ;
  typedef CGAL_SS_i::SS_converter<BaseC2C> C2C ;
  typedef CGAL_SS_i::SS_converter<BaseE2C> E2C ;

public:

  typedef Filtered_predicate<typename Exact    ::Get_ss_triedge_collinearity_2
                            ,typename Filtering::Get_ss_triedge_collinearity_2
                            , C2E
                            , C2F
                            >
                            Get_ss_triedge_collinearity_2 ;
  
  typedef Filtered_predicate<typename Exact    ::Compare_offset_against_event_time_2
                            ,typename Filtering::Compare_offset_against_event_time_2
                            , C2E
                            , C2F
                            >
                            Compare_offset_against_event_time_2 ;

  typedef CGAL_SS_i::Exceptionless_filtered_construction< typename Unfiltering::Construct_offset_point_2
                                                        , typename Exact      ::Construct_offset_point_2
                                                        , typename Unfiltering::Construct_offset_point_2
                                                        , C2E
                                                        , C2C
                                                        , E2C
                                                        , C2C
                                                        >
                                                        Construct_offset_point_2 ;
                                             
  typedef typename Unfiltering::Construct_ss_triedge_2        Construct_ss_triedge_2 ;
  typedef typename Unfiltering::Construct_ss_sorted_triedge_2 Construct_ss_sorted_triedge_2 ;
} ;

template<class K>
class Polygon_offset_builder_traits_2
  : public Polygon_offset_builder_traits_2_impl<typename CGAL_SS_i::Is_filtering_kernel<K>::type,K>
{
} ;

CGAL_STRAIGHT_SKELETON_CREATE_FUNCTOR_ADAPTER(Compare_offset_against_event_time_2);
CGAL_STRAIGHT_SKELETON_CREATE_FUNCTOR_ADAPTER(Construct_offset_point_2);

CGAL_END_NAMESPACE


#endif // CGAL_POLYGON_OFFSET_BUILDER_TRAITS_2_H //
// EOF //
